#pragma once

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "../payloads/ServiceRegistryEntry.h"

#include "core/Core.h"
#include "../utils/DbWrapper.h"

template<typename DB>
class Register {

    private:
        DB &db;

    public:

        Register(DB &db) : db{ db } {}

        Response processRegister(Request &&req)
        {
            bool bSuccessfullyParsed = false;
            ServiceRegistryEntry oServiceRegistryEntry;
            oServiceRegistryEntry.setJsonPayload(req.content, bSuccessfullyParsed);

            if(!bSuccessfullyParsed)
                return Response::from_stock(http::status_code::NotAcceptable);

            if(oServiceRegistryEntry.validRegistryEntry())
                oServiceRegistryEntry.parseRegistryEntry();
            else
                return Response::from_stock(http::status_code::NotAcceptable);

            //Service definition exists? if No -> save service Definition into DBase
            //TABLE: service_definition
            //
            DbWrapper<DB> dbw(db);
            std::string sServiceDefinitionID;
            dbw.checkAndInsertValue("id", "service_definition", "service_definition", oServiceRegistryEntry.sServiceDefinition, sServiceDefinitionID);
            //printf("sServiceDefinitionID: %s\n", sServiceDefinitionID.c_str());

            //
            //Provider/Requester system exists? if No -> save system into DBase
            //TABLE: system_
            //
            std::string sProviderSystemID;
            std::string sColumns = "system_name, address, port";
            std::string sValues = "'" +
                                    oServiceRegistryEntry.sProviderSystem_SystemName + "', '" +
                                    oServiceRegistryEntry.sProviderSystem_Address    + "', " +
                                    oServiceRegistryEntry.sProviderSystem_Port;

            if(oServiceRegistryEntry.sProviderSystem_AuthInfo.size())
            {
                sColumns += ", authentication_info";
                sValues += ", '" + oServiceRegistryEntry.sProviderSystem_AuthInfo + "'";
            }

            dbw.checkAndInsertValues(
                "id",
                "system_",
                "system_name",
                oServiceRegistryEntry.sProviderSystem_SystemName,
                sColumns,
                sValues,
                sProviderSystemID);

            //printf("sProviderSystemID: %s\n", sProviderSystemID.c_str());

            //
            //Interface exists? if No -> save interface into DBase
            //TABLE: service_interface
            //
            std::vector<std::string> vInterfaceIDs;

            for(int i = 0; i < oServiceRegistryEntry.vInterfaces.size(); ++i)
            {
                std::string sID;
                dbw.checkAndInsertValue("id", "service_interface", "interface_name", oServiceRegistryEntry.vInterfaces[i], sID);
                //printf("IntName: %s, ID: %s\n", oServiceRegistryEntry.vInterfaces[i].c_str(), sID.c_str());
                vInterfaceIDs.push_back(sID);
            }

            //
            //Save service registry entry into DBase
            //TABLE: service_registry
            //
            std::string sServiceRegistryEntryID;
            sColumns = "service_id, system_id, service_uri, end_of_validity, secure, metadata, version";
            sValues = sServiceDefinitionID + ", " +
                      sProviderSystemID + ", " +
                      "'" + oServiceRegistryEntry.sServiceUri + "', "+
                      "'" + oServiceRegistryEntry.sEndOfValidity + "', " +
                      "'" + oServiceRegistryEntry.sSecure     + "', "+
                      "'" + oServiceRegistryEntry.sMetaData   + "', " +
                      oServiceRegistryEntry.sVersion;

            dbw.checkAndInsertValues(
                "id",
                "service_registry",
                "service_uri",
                oServiceRegistryEntry.sServiceUri,
                sColumns,
                sValues,
                sServiceRegistryEntryID
            );

            //printf("ServiceRegistry ID: %s\n\n\n", sServiceRegistryEntryID.c_str());

            //
            //Save interface connection
            //TABLE: service_registry_interface_connection
            //
            for(uint i = 0; i < vInterfaceIDs.size(); ++i)
            {
                std::string sServiceRegistryIntfConnectionEntryID;

                sColumns = "service_registry_id, interface_id";
                sValues = sServiceRegistryEntryID + ", " + vInterfaceIDs[i];

                dbw.checkAndInsertValues(
                    "id",
                    "service_registry_interface_connection",
                    "service_registry_id",
                    sServiceRegistryEntryID,
                    sColumns,
                    sValues,
                    sServiceRegistryIntfConnectionEntryID
                );
            }

            //create json response from database response

            oServiceRegistryEntry.sQData.sId = sServiceRegistryEntryID;

            std::string sQuery = "SELECT * FROM service_definition where id = " + sServiceDefinitionID + ";";

            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                oServiceRegistryEntry.sQData.sServiceDefinition_id = sServiceDefinitionID;
                row->get(1, oServiceRegistryEntry.sQData.sServiceDefinition_serviceDefinition);
                row->get(2, oServiceRegistryEntry.sQData.sServiceDefinition_createdAt);
                row->get(3, oServiceRegistryEntry.sQData.sServiceDefinition_updatedAt);
            }
            else
            {
                return Response{ "Error: Empty response from service_definition table" };
            }

            sQuery = "SELECT * FROM system_ where id = " + sProviderSystemID + ";";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                oServiceRegistryEntry.sQData.sProvider_id = sProviderSystemID;
                row->get(1, oServiceRegistryEntry.sQData.sProvider_systemName);
                row->get(2, oServiceRegistryEntry.sQData.sProvider_address);
                row->get(3, oServiceRegistryEntry.sQData.sProvider_port);
                row->get(4, oServiceRegistryEntry.sQData.sProvider_authenticationInfo);
                row->get(5, oServiceRegistryEntry.sQData.sProvider_createdAt);
                row->get(6, oServiceRegistryEntry.sQData.sProvider_updatedAt);
            }
            else
            {
                return Response{ "Error: Empty response from system_ table" };
            }

            for(uint i = 0; i < vInterfaceIDs.size(); ++i)
            {
                sQuery = "SELECT * FROM service_interface where id = " + vInterfaceIDs[i] + ";";

                if (auto row = db.fetch(sQuery.c_str()) )
                {
                    oServiceRegistryEntry.sQData.vInterfaces_id.push_back(vInterfaceIDs[i]);
                    std::string s, c, u;

                    row->get(1, s);
                    oServiceRegistryEntry.sQData.vInterfaces_interfaceName.push_back(s.size() ? s : "");

                    row->get(2, c);
                    oServiceRegistryEntry.sQData.vInterfaces_createdAt.push_back(c.size() ? c : "");

                    row->get(3, u);
                    oServiceRegistryEntry.sQData.vInterfaces_updatedAt.push_back(u.size() ? u : "");
                }
                else
                {
                    return Response{ "Error: Empty response from service_interface table"};
                }
            }

            sQuery = "SELECT * FROM service_registry where id = " + sServiceRegistryEntryID + ";";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(3, oServiceRegistryEntry.sQData.sServiceUri);
                row->get(4, oServiceRegistryEntry.sQData.sEndOfValidity);
                row->get(5, oServiceRegistryEntry.sQData.sSecure);
                row->get(6, oServiceRegistryEntry.sQData.sMetadata);
                row->get(7, oServiceRegistryEntry.sQData.sVersion);
                row->get(8, oServiceRegistryEntry.sQData.sCreatedAt);
                row->get(9, oServiceRegistryEntry.sQData.sUpdatedAt);
            }
            else
            {
                return Response{ "Error: Empty response from service_registry table" };
            }

            std::string res = oServiceRegistryEntry.createRegistryEntry();

            //printf("res: \n%s\n", res.c_str());

            return Response{ res };
        }

};
