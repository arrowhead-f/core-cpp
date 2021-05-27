
#ifndef _ENDPOINTS_MGMTGET_H_
#define _ENDPOINTS_MGMTGET_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "core/Core.h"
#include "../utils/DbWrapper.h"
#include "../utils/Error.h"
#include "../payloads/SRSystem.h"
#include "../payloads/ServiceRegistryEntry.h"

template<typename DB>
class MgmtGet {

    private:
        DB &db;

    public:

        MgmtGet(DB &db) : db{ db } {}

        Response processMgmtGetId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();

            ServiceRegistryEntry oServiceRegistryEntry;
            uint8_t status = processServiceRegistryEntry(_Id, oServiceRegistryEntry);

            switch(status)
            {
                case 1: return ErrorResp{"Empty response from service_registry table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
                case 2: return ErrorResp{"Empty response from service_definition table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
                case 3: return ErrorResp{"Empty response from system_ table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
                case 4: return ErrorResp{"Empty response from service_registry_interface_connection table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
                case 5: return ErrorResp{"Empty response from service_interface table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
            }

            return Response{ oServiceRegistryEntry.createRegistryEntry() };
        }

        uint8_t processServiceRegistryEntry(int _Id, ServiceRegistryEntry &_roServiceRegistryEntry)
        {
            std::string sServiceRegistryEntryID = std::to_string(_Id);
            std::string sServiceDefinitionID;
            std::string sProviderSystemID;
            std::vector<std::string> vsInterfaceIDs;

            _roServiceRegistryEntry.sQData.sId = sServiceRegistryEntryID;

            std::string sQuery = "SELECT * FROM service_registry where id = '" + sServiceRegistryEntryID + "';";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(1, sServiceDefinitionID);
                row->get(2, sProviderSystemID);
                row->get(3, _roServiceRegistryEntry.sQData.sServiceUri);
                row->get(4, _roServiceRegistryEntry.sQData.sEndOfValidity);
                row->get(5, _roServiceRegistryEntry.sQData.sSecure);
                row->get(6, _roServiceRegistryEntry.sQData.sMetadata);
                row->get(7, _roServiceRegistryEntry.sQData.sVersion);
                row->get(8, _roServiceRegistryEntry.sQData.sCreatedAt);
                row->get(9, _roServiceRegistryEntry.sQData.sUpdatedAt);
            }
            else
            {
                 return 1;
            }

            sQuery = "SELECT * FROM service_definition where id = " + sServiceDefinitionID + ";";

            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                _roServiceRegistryEntry.sQData.sServiceDefinition_id = sServiceDefinitionID;
                row->get(1, _roServiceRegistryEntry.sQData.sServiceDefinition_serviceDefinition);
                row->get(2, _roServiceRegistryEntry.sQData.sServiceDefinition_createdAt);
                row->get(3, _roServiceRegistryEntry.sQData.sServiceDefinition_updatedAt);
            }
            else
            {
                 return 2;
            }

            sQuery = "SELECT * FROM system_ where id = " + sProviderSystemID + ";";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                _roServiceRegistryEntry.sQData.sProvider_id = sProviderSystemID;
                row->get(1, _roServiceRegistryEntry.sQData.sProvider_systemName);
                row->get(2, _roServiceRegistryEntry.sQData.sProvider_address);
                row->get(3, _roServiceRegistryEntry.sQData.sProvider_port);
                row->get(4, _roServiceRegistryEntry.sQData.sProvider_authenticationInfo);
                row->get(5, _roServiceRegistryEntry.sQData.sProvider_createdAt);
                row->get(6, _roServiceRegistryEntry.sQData.sProvider_updatedAt);
            }
            else
            {
                return 3;
            }

            sQuery = "SELECT interface_id FROM service_registry_interface_connection WHERE service_registry_id = '" + sServiceRegistryEntryID + "';";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    std::string s;
                    row->get(0, s);
                    vsInterfaceIDs.push_back(s);
                } while( row->next() );
            }
            else
            {
                return 4;
            }

            for(uint i = 0; i < vsInterfaceIDs.size(); ++i)
            {
                sQuery = "SELECT * FROM service_interface where id = " + vsInterfaceIDs[i] + ";";
                if (auto row = db.fetch(sQuery.c_str()) )
                {
                    std::string s;
                    _roServiceRegistryEntry.sQData.vInterfaces_id.push_back(vsInterfaceIDs[i]);
                    row->get(1, s);
                    _roServiceRegistryEntry.sQData.vInterfaces_interfaceName.push_back(s);
                    row->get(2, s);
                    _roServiceRegistryEntry.sQData.vInterfaces_createdAt.push_back(s);
                    row->get(3, s);
                    _roServiceRegistryEntry.sQData.vInterfaces_updatedAt.push_back(s);
                }
                else
                {
                    return 5;
                }
            }

            return 0;
        }

        Response processMgmtGetSystemsId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/systems/{id}"}.getResp();

            SRSystem oSRSystem;
            uint8_t status = processSystem(_Id, oSRSystem);

            if(status)
                return ErrorResp{"System with id " + std::to_string(_Id) + " not found.", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems/{id}"}.getResp();

            return Response{ oSRSystem.createSRSystem() };
        }

        uint8_t processSystem(int _Id, SRSystem &_roSystem)
        {
            std::string sQuery = "SELECT * FROM system_ WHERE id = '" + std::to_string(_Id) + "';";
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                std::string s;
                row->get(0, _roSystem.sId);
                row->get(1, _roSystem.sSystemName);
                row->get(2, _roSystem.sAddress);
                row->get(3, _roSystem.sPort);
                row->get(4, _roSystem.sAuthInfo);
                row->get(5, _roSystem.sCreatedAt);
                row->get(6, _roSystem.sUpdatedAt);
                return 0;
            }
            return 1;
        }

        Response processMgmtGetServicesId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();

            return Response{ "in progress" };
        }


};

#endif   /* _ENDPOINTS_MGMTGET_H_ */
