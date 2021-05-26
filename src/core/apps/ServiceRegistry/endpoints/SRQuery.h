#ifndef _ENDPOINTS_SRQUERY_H_
#define _ENDPOINTS_SRQUERY_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "../payloads/ServiceQueryForm.h"
#include "../payloads/ServiceQueryList.h"
#include "../payloads/SRSystem.h"

#include "core/Core.h"
#include "../utils/DbWrapper.h"
#include "../utils/Error.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

template<typename DB>
class SRQuery : SRPayloads
{

    private:
        DB &db;

        ServiceQueryForm oServiceQueryForm;
        ServiceQueryList oServiceQueryList;

    public:
        SRQuery(DB &db) : db{ db } {}

        Response processQuery(Request &&req)
        {
            if(!oServiceQueryForm.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", "serviceregistry/query"}.getResp();

            std::string errResp;
            uint8_t status = oServiceQueryForm.parseQueryForm(errResp);

            switch(status)
            {
                case 1: return ErrorResp{"Service definition requirement is null or blank", 400, "BAD_PAYLOAD", "serviceregistry/query"}.getResp();
                case 2: return ErrorResp{"Service definition requirement is null or blank", 400, "BAD_PAYLOAD", "serviceregistry/query"}.getResp();
                case 3: return ErrorResp{"Specified interface name is not valid: " + errResp, 400, "BAD_PAYLOAD", "serviceregistry/query"}.getResp();
                case 4: return ErrorResp{"Security type is not valid.", 400, "BAD_PAYLOAD", "serviceregistry/query"}.getResp();
            }

            status = fillQueryList();

            if (status != 0 )
            {
                if(status == 1)
                    return ErrorResp{"Service with definition of '" + oServiceQueryForm.sServiceDefinition + "' is not exists", 400, "INVALID_PARAMETER", "serviceregistry/query"}.getResp();
                else
                    return Response{ " { \"serviceQueryData\": [], \"unfilteredHits\": 0 } "};
            }

            if(oServiceQueryList.vServQueryData.size() == 0)
                return Response{ " { \"serviceQueryData\": [], \"unfilteredHits\": 0 } "};

            filterQueryList();

            return Response{ oServiceQueryList.createServiceQueryList() };
        }

        int fillQueryList()
        {
            oServiceQueryList.hits = 0;
            serviceQueryData sQData;
//
// QUERY service_definition
//
            std::string sQuery = "SELECT * FROM service_definition WHERE service_definition = '" + oServiceQueryForm.sServiceDefinition + "';";
            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, sQData.sServiceDefinition_id);
                row->get(1, sQData.sServiceDefinition_serviceDefinition);
                row->get(2, sQData.sServiceDefinition_createdAt);
                row->get(3, sQData.sServiceDefinition_updatedAt);
            }
            else
            {
                return 1;
            }
//
// QUERY service_registry
//
            sQuery = "SELECT * FROM service_registry WHERE service_id = '" + sQData.sServiceDefinition_id + "';";
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    row->get(0, sQData.sId);
//
// QUERY service_registry_interface_connection
//
                    sQData.vInterfaces_id.clear();
                    sQData.vInterfaces_interfaceName.clear();
                    sQData.vInterfaces_createdAt.clear();
                    sQData.vInterfaces_updatedAt.clear();

                    sQuery = "SELECT * FROM service_registry_interface_connection WHERE service_registry_id = '" + sQData.sId + "';";

                    if (auto row2 = db.fetch(sQuery.c_str()) )
                    {
                        do{
                            std::string intfID;
                            row2->get(2, intfID);
//
// QUERY service_interface
//
                            sQuery = "SELECT * FROM service_interface WHERE id = '" + intfID + "';";
                            if ( auto row3 = db.fetch(sQuery.c_str()) )
                            {
                                std::string sIntfId;
                                std::string sIntfName;
                                std::string sCAt;
                                std::string sUAt;
                                row3->get(0, sIntfId);
                                row3->get(1, sIntfName);
                                row3->get(2, sCAt);
                                row3->get(3, sUAt);

                                sQData.vInterfaces_id.push_back(sIntfId);
                                sQData.vInterfaces_interfaceName.push_back(sIntfName);
                                sQData.vInterfaces_createdAt.push_back(sCAt);
                                sQData.vInterfaces_updatedAt.push_back(sUAt);
                            }
                            else
                            {
                                return 2;
                            }

                        } while( row2->next() );
                    }
                    else
                    {
                        return 3;
                    }
//
// QUERY system_
//
                    std::string systemID;
                    row->get(2, systemID);

                    sQuery = "SELECT * FROM system_ WHERE id = '" + systemID + "';";
                    if ( auto row2 = db.fetch(sQuery.c_str()) )
                    {
                        row2->get(0, sQData.sProvider_id);
                        row2->get(1, sQData.sProvider_systemName);
                        row2->get(2, sQData.sProvider_address);
                        row2->get(3, sQData.sProvider_port);
                        row2->get(4, sQData.sProvider_authenticationInfo);
                        row2->get(5, sQData.sProvider_createdAt);
                        row2->get(6, sQData.sProvider_updatedAt);
                    }
                    else
                    {
                        return 4;
                    }

                    row->get(3, sQData.sServiceUri);
                    row->get(4, sQData.sEndOfValidity);
                    row->get(5, sQData.sSecure);
                    row->get(6, sQData.sMetadata);
                    row->get(7, sQData.sVersion);
                    row->get(8, sQData.sCreatedAt);
                    row->get(9, sQData.sUpdatedAt);

                    oServiceQueryList.vServQueryData.push_back(sQData);
                    oServiceQueryList.hits++;
                } while( row->next() );
            }
            else
            {
                return 5;
            }

            return 0;
        }

        void filterQueryList()
        {
            for ( auto it = oServiceQueryList.vServQueryData.begin(); it != oServiceQueryList.vServQueryData.end(); )
            {
                //
                // Filter on interfaces
                //
                bool match = oServiceQueryForm.vInterfaceRequirements.size() ? false : true;

                for ( std::string intf : oServiceQueryForm.vInterfaceRequirements )
                    for ( std::string dbIntf : it->vInterfaces_interfaceName )
                        if ( intf.compare(dbIntf) == 0 )
                            match = true;

                if( !match )
                {
                    it = oServiceQueryList.vServQueryData.erase(it);
                    continue;
                }

                //
                // Filter on security type
                //
                match = oServiceQueryForm.vSecurityRequirements.size() ? false : true;

                for ( std::string secType : oServiceQueryForm.vSecurityRequirements )
                    if ( secType.compare(it->sSecure) == 0 )
                        match = true;

                if( !match )
                {
                    it = oServiceQueryList.vServQueryData.erase(it);
                    continue;
                }

                //
                // Filter on version
                //
                if(oServiceQueryForm.bVersionReqExists)
                {
                    if( oServiceQueryForm.iVersionReq != std::stoi(it->sVersion))
                    {
                        it = oServiceQueryList.vServQueryData.erase(it);
                        continue;
                    }
                }

                if( oServiceQueryForm.bMinVersionReqExists )
                {
                    if( oServiceQueryForm.iMinVersionReq > std::stoi(it->sVersion))
                    {
                        it = oServiceQueryList.vServQueryData.erase(it);
                        continue;
                    }
                }

                if( oServiceQueryForm.bMaxVersionReqExists )
                {
                    if( oServiceQueryForm.iMaxVersionReq < std::stoi(it->sVersion))
                    {
                        it = oServiceQueryList.vServQueryData.erase(it);
                        continue;
                    }
                }

                //
                // Filter on metadata
                //
                std::map<std::string, std::string> mDbMeta;
                std::string token;
                std::istringstream tokenStream(it->sMetadata);
                while(std::getline(tokenStream, token, ','))
                {
                    std::istringstream tokenStream2(token);
                    std::string key;
                    std::string value;
                    std::getline(tokenStream2, key, '=');
                    if ( key.size() != 0)
                    {
                        std::getline(tokenStream2, value, '=');
                        if ( value.size() != 0)
                            mDbMeta.insert({key, value});
                    }
                }

                match = true;

                std::map<std::string,std::string>::iterator iter;
                for (const auto [key, value] : oServiceQueryForm.mMetadataRequirements)
                {
                    iter = mDbMeta.find(key);
                    if(iter != mDbMeta.end())
                    {
                        if(value.compare(mDbMeta[key]) != 0)
                        {
                            match = false;
                            break;
                        }
                    }
                    else
                    {
                        match = false;
                        break;
                    }
                }

                if( !match )
                {
                    it = oServiceQueryList.vServQueryData.erase(it);
                    continue;
                }

                //
                // Ping provider - try to connect through TCP
                //
                if(oServiceQueryForm.bPingProviders)
                {
                    struct sockaddr_in server;

                    int sock = socket(AF_INET , SOCK_STREAM , 0);
                    if (sock == -1)
                    {

                        it = oServiceQueryList.vServQueryData.erase(it);
                        continue;
                    }

                	server.sin_addr.s_addr = inet_addr(it->sProvider_address.c_str());
                	server.sin_family = AF_INET;
                	server.sin_port = htons( std::stoi(it->sProvider_port) );

                	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) //Connection failed
                	{
                        it = oServiceQueryList.vServQueryData.erase(it);
                        continue;
                	}

                    close(sock);
                }

                ++it;
            }
        }

        Response processQuerySystem(Request &&req)
        {
            SRSystem oSRSystem;
            if(!oSRSystem.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", "serviceregistry/query/system"}.getResp();

            uint8_t status = oSRSystem.validSystem();

            switch(status)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5: return ErrorResp{"parameter null or empty", 400, "INVALID_PARAMETER", "serviceregistry/query/system"}.getResp();
                case 6: return ErrorResp{"Port must be between 0 and 65535.", 400, "INVALID_PARAMETER", "serviceregistry/query/system"}.getResp();
            }

            std::string sQuery = "SELECT * FROM system_ where system_name = '" + oSRSystem.sSystemName +
                                 "' AND address = '" + oSRSystem.sAddress +
                                 "' AND port = '"    + oSRSystem.sPort    + "';";

            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, oSRSystem.sId);
                row->get(4, oSRSystem.sAuthInfo);
                row->get(5, oSRSystem.sCreatedAt);
                row->get(6, oSRSystem.sUpdatedAt);
            }
            else
            {
                return ErrorResp{"No system with name: " + oSRSystem.sSystemName + ", address: " + oSRSystem.sAddress + " and port: " + oSRSystem.sPort, 400, "INVALID_PARAMETER", "serviceregistry/query/system"}.getResp();
            }

            return Response{ oSRSystem.createSRSystem() };
        }

        Response processQuerySystemId(int _Id)
        {
            if (_Id < 0)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/query/system/{id}"}.getResp();

            std::string sQuery = "SELECT * FROM system_ where id = " + std::to_string(_Id) + ";";

            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                SRSystem oSRSystem;

                row->get(0, oSRSystem.sId);
                row->get(1, oSRSystem.sSystemName);
                row->get(2, oSRSystem.sAddress);
                row->get(3, oSRSystem.sPort);
                row->get(4, oSRSystem.sAuthInfo);
                row->get(5, oSRSystem.sCreatedAt);
                row->get(6, oSRSystem.sUpdatedAt);

                return Response{ oSRSystem.createSRSystem() };
            }
            else
            {
                return ErrorResp{"System with id " + std::to_string(_Id) + " not found.", 400, "INVALID_PARAMETER", "serviceregistry/query/system/{id}"}.getResp();
            }
        }

};

#endif   /* _ENDPOINTS_SRQUERY_H_ */
