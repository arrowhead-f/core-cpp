#ifndef _ARROWHEAD_SERVICEREGISTRY_H_
#define _ARROWHEAD_SERVICEREGISTRY_H_

#include "core/Core.h"

#include <cstring>

#include "../../json/ServiceQueryForm.hpp"
#include "../../json/ServiceQueryList.hpp"
#include "../../json/ServiceRegistryEntry.hpp"
#include "../../json/System.h"
#include "../../json/ServiceDefinition.h"

#define ECHO                 0
#define QUERY                1
#define REGISTER             2
#define UNREGISTER           3
#define MGMT                 4
#define SYSTEM               5
#define GROUPED              6
#define SERVICEDEF           7
#define SERVICES             8
#define SYSTEMS              9
#define MAX_SUPPORTED_PATHS 10
#define UNKNOWNSUBPATH      11

template<typename DBPool>class ServiceRegistry final : public Core<DBPool> {

private:
    std::vector<std::string> vsSubPath;
    std::vector<int> viSubPath;

public:
    using Core<DBPool>::Core;

    void parseURL(const char *_szURL)
    {
        std::string subUrls[MAX_SUPPORTED_PATHS] = {
            "echo",
            "query",
            "register",
            "unregister",
            "mgmt",
            "system",
            "grouped",
            "servicedef",
            "services",
            "systems"
        };

        printf("%s\n",_szURL);
        vsSubPath.clear();
        viSubPath.clear();

        for(char *pch = strtok((char *)_szURL, "/"); pch != NULL; pch = strtok(NULL, "/"))
            vsSubPath.push_back((string)pch);

        for(int i = 0; i < vsSubPath.size(); ++i)
        {
            bool found = false;
            for(int j = 0; j < MAX_SUPPORTED_PATHS; ++j)
            {
                if( strcmp(vsSubPath[i].c_str(), (char *)subUrls[j].c_str()) == 0 )
                {
                    found = true;
                    viSubPath.push_back(j);
                    break;
                }
            }
            if(!found)
                viSubPath.push_back(UNKNOWNSUBPATH);
        }

    }

/*
    void printParsedURL()
    {
        for(int i=0; i<vsSubPath.size();++i)
            printf("%d - %s\n",i,vsSubPath[i].c_str());

        printf("\n");
        for(int i=0; i<viSubPath.size(); ++i)
            printf("%d - %d\n", i, viSubPath[i]);
    }
*/

////////////////////////////////////////
// DB
///////////////////////////////////////

    bool dbContentExists(std::string _sSelect, std::string _sFrom, std::string _sWhere, std::string _sWhat)
    {
        auto db = Core<DBPool>::database();
        std::string sQuery = "SELECT ";
        sQuery += _sSelect + " FROM " + _sFrom + " WHERE " + _sWhere + " = '" + _sWhat + "';";
        printf("Query: %s\n", sQuery.c_str());
        std::string sResponse;
        db.fetch(sQuery.c_str(), sResponse);
        printf("sReponse: %s\n", sResponse.c_str());
        return sResponse.size() != 0;
    }

    bool dbContentExists(std::string _sSelect, std::string _sFrom,std::string _sWhere, std::string _sWhat, std::string &_sResponse)
    {
        auto db = Core<DBPool>::database();
        std::string sQuery = "SELECT ";
        sQuery += _sSelect + " FROM " + _sFrom + " WHERE " + _sWhere + " = '" + _sWhat + "';";
        printf("Query: %s\n", sQuery.c_str());
        db.fetch(sQuery.c_str(), _sResponse);
        printf("_sReponse: %s\n", _sResponse.c_str());
        return _sResponse.size() != 0;
    }

    void dbSaveContent(std::string _sTableName, std::string _sColumn, std::string _sValue, bool bMore)
    {
        auto db = Core<DBPool>::database();
        std::string sQuery = "INSERT INTO ";
        if(!bMore)
            sQuery += _sTableName + " (" + _sColumn + ") " + "VALUES ('" + _sValue + "');";
        else
            sQuery += _sTableName + " (" + _sColumn + ") " + "VALUES (" + _sValue + ");";
        printf("Query: %s\n", sQuery.c_str());
        db.query(sQuery.c_str());
    }

    void checkAndInsertValue(std::string _sReturnColumn, std::string _sTable, std::string _sCheckColumn, std::string _sCheckValue, std::string &_rRetValue)
    {
        if( !dbContentExists(_sReturnColumn, _sTable, _sCheckColumn, _sCheckValue, _rRetValue))
        {
            dbSaveContent(_sTable, _sCheckColumn, _sCheckValue, false);
            //todo: get ID as db.query() response!
            dbContentExists(_sReturnColumn, _sTable, _sCheckColumn, _sCheckValue, _rRetValue);
        }
    }

    void checkAndInsertValues(
        std::string _sReturnColumn,
        std::string _sTable,
        std::string _sCheckColumn,
        std::string _sCheckValue,
        std::string _sInsertColumn,
        std::string _sInsertValue,
        std::string &_rRetValue)
    {
        if( !dbContentExists(_sReturnColumn, _sTable, _sCheckColumn, _sCheckValue, _rRetValue))
        {
            dbSaveContent(_sTable, _sInsertColumn, _sInsertValue, true);
            //todo: get ID as db.query() response!
            dbContentExists(_sReturnColumn, _sTable, _sCheckColumn, _sCheckValue, _rRetValue);
        }
    }

////////////////////////////////////////
// LOGIC
///////////////////////////////////////

// Management functions

// DELETE

    std::string processMgmtDeleteId(std::string _sId)
    {
        std::string sQuery = "DELETE FROM service_registry WHERE id = '" + _sId + "';";
        auto db = Core<DBPool>::database();
        db.query(sQuery.c_str());
        return "OK";
    }

    std::string processMgmtDeleteServicesId(std::string _sId)
    {
        std::string sQuery = "DELETE FROM service_definition WHERE id = '" + _sId + "';";
        auto db = Core<DBPool>::database();
        db.query(sQuery.c_str());
        return "OK";
    }

    std::string processMgmtDeleteSystemsId(std::string _sId)
    {
        std::string sQuery = "DELETE FROM system_ WHERE id = '" + _sId + "';";
        auto db = Core<DBPool>::database();
        db.query(sQuery.c_str());
        return "OK";
    }

// PUT or PATCH

    std::string processMgmtPutOrPatch(std::string _sId, const char* _szPayload, bool _bCheckMandatoryFields)
    {
        return "todo";
    }

    std::string processMgmtPutOrPatchServices(std::string _sId, const char* _szPayload, bool _bCheckMandatoryFields)
    {
        // Input: ID and ServiceDefinition
        bool bSuccess = false;
        ServiceDefinition oServiceDefinition(_szPayload, bSuccess);
        if( !bSuccess ) return "Error: Invalid JSon format!";

        bool bUpdate = false;

        if(_bCheckMandatoryFields)
        {
            if( !oServiceDefinition.validJSONPayload() )
                return "Error: Missing information!";
            bUpdate = true;
        }
        else
        {
            if( !getValue(oServiceDefinition.mainObject, "serviceDefinition", oServiceDefinition.sServiceDefinition ) )
                bUpdate = false;
            bUpdate = true;
        }

        if(bUpdate)
        {
            std::string sQuery = "UPDATE service_definition SET service_definition = '" +
                     oServiceDefinition.sServiceDefinition + "' WHERE id = '" + _sId + "';";

            auto db = Core<DBPool>::database();
            db.query(sQuery.c_str());
        }

        return processMgmtGetServicesId(_sId);
    }

    std::string processMgmtPutOrPatchSystems(std::string _sId, const char* _szPayload, bool _bCheckMandatoryFields)
    {
        // Input:  System
        bool bSuccess = false;
        System oSystem(_szPayload, bSuccess);
        if( !bSuccess ) return "Error: Invalid JSon format!";

        if(_bCheckMandatoryFields)
        {
            if( !oSystem.validJSONPayload() ) return "Error: Missing information!";
            std::string sQuery = "UPDATE system_ SET system_name = '" + oSystem.sSystemName + "', " +
                                 "address = '" + oSystem.sAddress + "', " +
                                 "port = '" + oSystem.sPort + "', ";

            sQuery += getValue(oSystem.mainObject, "authenticationInfo", oSystem.sAuthInfo ) ? std::string("authentication_info = '" + oSystem.sAuthInfo + "' ") : std::string("authentication_info = 'NULL' ");
            sQuery += " WHERE id = '" + _sId + "';";

            auto db = Core<DBPool>::database();
            db.query(sQuery.c_str());
        }
        else
        {
            bool bName     = getValue(oSystem.mainObject, "systemName"        , oSystem.sSystemName);
            bool bAddr     = getValue(oSystem.mainObject, "address"           , oSystem.sAddress);
            bool bPort     = getValue(oSystem.mainObject, "port"              , oSystem.sPort);
            bool bAuthInfo = getValue(oSystem.mainObject, "authenticationInfo", oSystem.sAuthInfo);

            std::string sQuery = "UPDATE system_ SET ";
            if(bName) sQuery += "system_name = '" + oSystem.sSystemName + "'";

            if(bAddr & bName) sQuery += ", ";
            if(bAddr) sQuery += "address = '" + oSystem.sAddress + "'";

            if(bPort & (bAddr || (!bAddr & bName))) sQuery += ", ";
            if(bPort) sQuery += "port = '" + oSystem.sPort + "'";

            if(bAuthInfo & (bPort || (!bPort & bAddr) || (!bAddr & bName))) sQuery += ", ";
            if(bAuthInfo) sQuery += "authentication_info = '" + oSystem.sAuthInfo + "'";

            sQuery += " WHERE id = '" + _sId + "';";

            auto db = Core<DBPool>::database();
            db.query(sQuery.c_str());
        }

        return processMgmtGetSystemsId(_sId);
    }

// POST

    std::string processMgmtPost(const char* _szPayload)
    {
        // Input:  ServiceRegistryEntry
        // Output: ServiceRegistryEntry
        return "todo";
    }

    std::string processMgmtPostServices(const char* _szPayload)
    {
        // Input:  ServiceDefinition
        bool bSuccess = false;
        ServiceDefinition oServiceDefinition(_szPayload, bSuccess);
        if( !bSuccess ) return "Error: Invalid JSon format!";

        if( !oServiceDefinition.validJSONPayload() ) return "Error: Missing information!";

        std::string sId;
        checkAndInsertValue("id", "service_definition", "service_definition", oServiceDefinition.sServiceDefinition, sId);
        return processMgmtGetServicesId(sId);
    }

    std::string processMgmtPostSystems(const char* _szPayload)
    {
        // Input:  System
        bool bSuccess = false;
        System oSystem(_szPayload, bSuccess);
        if( !bSuccess ) return "Error: Invalid JSon format!";

        if( !oSystem.validJSONPayload() ) return "Error: Missing information!";

        std::string sQuery = "SELECT id FROM system_ where system_name = '" + oSystem.sSystemName + "' "
                             "AND address = '" + oSystem.sAddress + "' " +
                             "AND port = '" + oSystem.sPort + "' ;";

        std::string sId;
        auto db = Core<DBPool>::database();
        db.fetch(sQuery.c_str(), sId);

        if( sId.size() != 0 ) //content already exists
        {
            return processMgmtGetSystemsId(sId);
        }
        //else - insert new content

        dbSaveContent(  "system_",
                        "system_name, address, port, authentication_info",
                        "'" + oSystem.sSystemName + "', " +
                        "'" + oSystem.sAddress + "', " +
                        "'" + oSystem.sPort + "', " +
                        "'" + oSystem.sAuthInfo + "'",
                        true);

        // Output: System
        db.fetch(sQuery.c_str(), sId);
        return processMgmtGetSystemsId(sId);
    }

// GET

    std::string processMgmtGet()
    {
        ServiceRegistryEntryList oServiceRegistryEntryList;

        std::string sQuery = "SELECT id FROM service_registry";

        auto db = Core<DBPool>::database();

        if (auto row = db.fetch(sQuery.c_str()) )
        {
            oServiceRegistryEntryList.uCount = 0;
            do{
                std::string sID;
                ServiceRegistryEntry oServRegEntry;
                row->get(0, sID);
                oServiceRegistryEntryList.vServiceRegistryEntry.push_back(oServRegEntry);
                processServiceRegistryEntry(sID, oServiceRegistryEntryList.vServiceRegistryEntry[oServiceRegistryEntryList.uCount++]);
            } while( row->next() );
        }
        else
        {
            return "Warning: Empty response from service_registry table";
        }

        return oServiceRegistryEntryList.createRegistryEntryList();
    }

    std::string processMgmtGetId(std::string _sId)
    {
        ServiceRegistryEntry oServiceRegistryEntry;
        processServiceRegistryEntry(_sId, oServiceRegistryEntry);
        return oServiceRegistryEntry.createRegistryEntry();
    }

    void processServiceRegistryEntry(std::string _sId, ServiceRegistryEntry &_roServiceRegistryEntry)
    {
        std::string sServiceRegistryEntryID = _sId;
        std::string sServiceDefinitionID;
        std::string sProviderSystemID;
        std::vector<std::string> vsInterfaceIDs;
        std::string sQuery;

        _roServiceRegistryEntry.sQData.sId = sServiceRegistryEntryID;

        auto db = Core<DBPool>::database();

        sQuery = "SELECT * FROM service_registry where id = '" + sServiceRegistryEntryID + "';";

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
            return;
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
            return;
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
            return;
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
            return;
        }

        for(uint i = 0; i < vsInterfaceIDs.size(); ++i)
        {
            sQuery = "SELECT * FROM service_interface where id = " + vsInterfaceIDs[i] + ";";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                string s;
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
                return;
            }
        }
    }

    std::string processMgmtGetServiceDef(std::string _sServiceDefinition, const char *_pszPage, const char *_pszItemPerPage, const char *_pszSortField, const char *_pszDirection)
    {
        uint uPage = _pszPage == NULL ? 0 : atoi(_pszPage);
        uint uItemPerPage = _pszItemPerPage == NULL ? 0 : atoi(_pszItemPerPage);

        std::string sSortField;
        if(_pszSortField != NULL)
        {
            if( !((strcmp(_pszSortField, "id") == 0) || (strcmp(_pszSortField, "createdAt") == 0) || (strcmp(_pszSortField, "updatedAt") == 0)) )
            {
                return "Error: Unknown sort field!";
            }
            else
            {
                if( strcmp(_pszSortField, "createdAt") == 0 )
                    sSortField = "created_at";
                else if( strcmp(_pszSortField, "updatedAt") == 0 )
                    sSortField = "updated_at";
                else
                    sSortField = "id";
            }
        }

        std::string sDirection;
        if(_pszDirection != NULL)
        {
            if( !((strcmp(_pszDirection, "ASC") == 0) || (strcmp(_pszDirection, "DESC") == 0)) )
                return "Error: Unknown direction!";
            else
                sDirection = std::string(_pszDirection);
        }

        std::string sQuery = "SELECT id FROM service_definition WHERE service_definition = '" + _sServiceDefinition + "';";
        std::string sServiceDefinitionID;

        auto db = Core<DBPool>::database();
        db.fetch(sQuery.c_str(), sServiceDefinitionID);

        if(sServiceDefinitionID.size() == 0) return("Warning: Unknown Service Definition!");

        ServiceRegistryEntryList oServiceRegistryEntryList;

        sQuery = "SELECT id FROM service_registry WHERE service_id = '" + sServiceDefinitionID + "'";

        if(_pszSortField != NULL)
        {
            sQuery += "ORDER BY '" + sSortField + "' ";

            if(_pszDirection != NULL)
                sQuery += sDirection;
        }

        sQuery += ";";

        if (auto row = db.fetch(sQuery.c_str()) )
        {
            oServiceRegistryEntryList.uCount = 0;
            uint i = 0;
            do{
                if( (_pszItemPerPage != NULL) && (uItemPerPage == i)) break;
                if( i++ != uPage ) continue;

                std::string sID;
                ServiceRegistryEntry oServRegEntry;
                row->get(0, sID);
                oServiceRegistryEntryList.vServiceRegistryEntry.push_back(oServRegEntry);
                processServiceRegistryEntry(sID, oServiceRegistryEntryList.vServiceRegistryEntry[oServiceRegistryEntryList.uCount++]);
            } while( row->next() );
        }
        else
        {
            return "Warning: Empty response from service_registry table";
        }

        return oServiceRegistryEntryList.createRegistryEntryList();
    }

    std::string processMgmtGetServices()
    {
        ServiceDefinitionList oServiceDefinitionList;

        std::string sQuery = "SELECT id FROM service_definition";

        auto db = Core<DBPool>::database();

        if (auto row = db.fetch(sQuery.c_str()) )
        {
            oServiceDefinitionList.uCount = 0;
            do{
                std::string sID;
                ServiceDefinition oServiceDefinition;
                row->get(0, sID);
                oServiceDefinitionList.vServiceDefinition.push_back(oServiceDefinition);
                processServiceDefinition(sID, oServiceDefinitionList.vServiceDefinition[oServiceDefinitionList.uCount++]);
            } while( row->next() );
        }
        else
        {
            return "Warning: Empty response from service_definition table";
        }

        return oServiceDefinitionList.createServiceDefinitionList();
    }

    std::string processMgmtGetServicesId(std::string _sId)
    {
        ServiceDefinition oServiceDefinition;
        processServiceDefinition(_sId, oServiceDefinition);
        return oServiceDefinition.createServiceDefinition();
    }

    void processServiceDefinition(std::string _sId, ServiceDefinition &_roServiceDefinition)
    {
        std::string sQuery = "SELECT * FROM service_definition WHERE id = '" + _sId + "';";
        auto db = Core<DBPool>::database();
        if (auto row = db.fetch(sQuery.c_str()) )
        {
            std::string s;
            row->get(0, _roServiceDefinition.sId);
            row->get(1, _roServiceDefinition.sServiceDefinition);
            row->get(2, _roServiceDefinition.sCreatedAt);
            row->get(3, _roServiceDefinition.sUpdatedAt);
        }
    }

    std::string processMgmtGetSystems()
    {
        SystemList oSystemList;

        std::string sQuery = "SELECT id FROM system_";

        auto db = Core<DBPool>::database();

        if (auto row = db.fetch(sQuery.c_str()) )
        {
            oSystemList.uCount = 0;
            do{
                std::string sID;
                System oSystem;
                row->get(0, sID);
                oSystemList.vSystem.push_back(oSystem);
                processSystem(sID, oSystemList.vSystem[oSystemList.uCount++]);
            } while( row->next() );
        }
        else
        {
            return "Warning: Empty response from system_ table";
        }

        return oSystemList.createSystemList();
    }

    std::string processMgmtGetSystemsId(std::string _sId)
    {
        System oSystem;
        processSystem(_sId, oSystem);
        return oSystem.createSystem();
    }

    void processSystem(std::string _sId, System &_roSystem)
    {
        std::string sQuery = "SELECT * FROM system_ WHERE id = '" + _sId + "';";
        auto db = Core<DBPool>::database();
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
        }
    }

// Public and Private functions

    std::string processQuery(const char *_szPayload)
    {
/*
        bool bSuccessfullyParsed = false;
        ServiceQueryForm oServiceQueryForm(_szPayload, bSuccessfullyParsed);

        if(!bSuccessfullyParsed)
            return "Error: Bad JSON format!";

        if(!oServiceQueryForm.parseQueryForm())
            return "Error: Invalid ServiceQueryForm!";

        std::string sQuery = "SELECT id FROM service_definition WHERE service_definition = '" + + "';";
        std::string sServiceDefinitionID;

        auto db = Core<DBPool>::database();
        db.fetch(sQuery.c_str(), sServiceDefinitionID);
        if( sServiceDefinitionID.size() == '0' ) return "Warning: Unknown Service Definition";

        printf("sServiceDefinitionID: %s\n", sServiceDefinitionID.c_str());

        std::vector<std::string> vsInterfaceIDs;
        for(uint i = 0; i < oServiceQueryForm.vInterfaceRequirements.size(); ++i)
        {
            sQuery = "SELECT id FROM service_interface WHERE interface_name = '" + oServiceQueryForm.vInterfaceRequirements[i] + "';";
            db.fetch(sQuery.c_str(), vsInterfaceIDs[i]);
        }

        if(oServiceQueryForm.vInterfaceRequirements.size() != 0 && vsInterfaceIDs.size() == 0) return "Warning: Unknwn interface requirement(s)";

        for(uint i = 0; i < vsInterfaceIDs.size(); ++i)
            printf( "vsInterfaceIDs: %s\n", vsInterfaceIDs[i].c_str() );

        std::vector<std::string> vsServiceRegistryId;
        if(vsInterfaceIDs.size() != 0)
        {
            sQuery = "SELECT service_registry_id FROM service_registry_inerface_connection WHERE "
            for(uint i = 0; i < vsInterfaceIDs.size(); ++i)
            {
                sQuery += "interface_id = '" + vsInterfaceIDs[i] + "'";
                if(i != (vsInterfaceIDs.size() - 1) )
                    sQuery += " OR ";
                else
                    sQuery += ";";
            }

            if( auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    std::string sID;
                    row->get(0,sID)
                    vsServiceRegistryId.push_back(sID);
                } while( row->next() );

                if( vsServiceRegistryId.size() == 0 ) "Warning: Empty response from service_registry_interface_connection table";

                sQuery = "SELECT * FROM service_registry WHERE (";

                for(uint i = 0; i < vsServiceRegistryId.size(); ++i)
                {
                    sQuery += "id = '" + vsServiceRegistryId[i] + "' ";
                    if(i != (vsServiceRegistryId.size() - 1) )
                        sQuery += "OR ";
                    else
                        sQuery += ") ";
                }

                //Filter on security
                if( oServiceQueryForm.vSecurityRequirements.size() != 0) sQuery += "(";

                for(uint i = 0; i < oServiceQueryForm.vSecurityRequirements.size(); ++i)
                {
                    sQuery += "secure = '" + oServiceQueryForm.vSecurityRequirements[i] + "' ";
                    if(i != (vsServiceRegistryId.size() - 1) )
                        sQuery += "OR ";
                    else
                        sQuery += ") ";
                }

                //Filter on Metadata
                for (auto it = oServiceQueryForm.mMetadataRequirements.begin(); it != oServiceQueryForm.mMetadataRequirements.end(); ++it)
                    dbQuery += it->first + ":" + it->second + " ";
            }
            else
            {
                return "Warning: Empty response from service_registry_interface_connection table";
            }

        }
        else
        {
            sQuery = "SELECT * FROM service_registry WHERE service_id = '" + sServiceDefinitionID + "'";
        }

*/

/*



    //Filter on Version
        if(oServiceQueryForm.sVersionReq.length())    dbQuery += oServiceQueryForm.sVersionReq + " ";
        if(oServiceQueryForm.sMinVersionReq.length()) dbQuery += oServiceQueryForm.sMinVersionReq + " ";
        if(oServiceQueryForm.sMaxVersionReq.length()) dbQuery += oServiceQueryForm.sMaxVersionReq + " ";

    //Filter on Ping ???
        if(oServiceQueryForm.sPingProviders.length()) dbQuery += oServiceQueryForm.sPingProviders + " ";

    //todo: DBQuery
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
          return "Error: Empty response from service_definition table";
      }
*/
    //todo: fill response content from DB answer

        ServiceQueryList oServiceQueryList; //this will be the answer Json content
        oServiceQueryList.fillContent();
        return oServiceQueryList.createServiceQueryList();
    }

    std::string processQuerySystem(const char *_szPayload)
    {
        bool bSuccessfullyParsed = false;

        System oSystem(_szPayload, bSuccessfullyParsed);

        if( !bSuccessfullyParsed ) return "Error: Bad JSON payload!";

        if( !oSystem.validJSONPayload() ) return "Error: Invalid JSON content!";

        std::string sQuery = "SELECT * FROM system_ where system_name = '" + oSystem.sSystemName +
                             "' AND address = '" + oSystem.sAddress +
                             "' AND port = '"    + oSystem.sPort    + "';";

        auto db = Core<DBPool>::database();

        if ( auto row = db.fetch(sQuery.c_str()) )
        {
            row->get(0, oSystem.sId);
            //row->get(1, oSystem.sSystemName); //validJSONPayload() also fills this string
            //row->get(2, oSystem.sAddress);    //validJSONPayload() also fills this string
            //row->get(3, oSystem.sPort);       //validJSONPayload() also fills this string
            row->get(4, oSystem.sAuthInfo);
            row->get(5, oSystem.sCreatedAt);
            row->get(6, oSystem.sUpdatedAt);

            return oSystem.createSystem();
        }
        else
        {
            return "Error: Empty response from system_ table";
        }

    }

    std::string processQuerySystemId(std::string _sId)
    {
        auto db = Core<DBPool>::database();
        std::string sQuery = "SELECT * FROM system_ where id = " + _sId + ";";

        if ( auto row = db.fetch(sQuery.c_str()) )
        {
            System oSystem;

            row->get(0, oSystem.sId);
            row->get(1, oSystem.sSystemName);
            row->get(2, oSystem.sAddress);
            row->get(3, oSystem.sPort);
            row->get(4, oSystem.sAuthInfo);
            row->get(5, oSystem.sCreatedAt);
            row->get(6, oSystem.sUpdatedAt);

            return oSystem.createSystem();
        }
        else
        {
            return "Error: Empty response from system_ table";
        }
    }

    std::string processRegister(const char *_szPayload)
    {
        bool bSuccessfullyParsed = false;
        ServiceRegistryEntry oServiceRegistryEntry(_szPayload, bSuccessfullyParsed);

        if(!bSuccessfullyParsed)
            return "Error: Bad Json format!";

        if(!oServiceRegistryEntry.parseRegistryEntry())
            return "Error: Invalid ServiceRegistryEntry!";

    //
    //Service definition exists? if No -> save service Definition into DBase
    //TABLE: service_definition
    //
        std::string sServiceDefinitionID;
        checkAndInsertValue("id", "service_definition", "service_definition", oServiceRegistryEntry.sServiceDefinition, sServiceDefinitionID);
        printf("sServiceDefinitionID: %s\n", sServiceDefinitionID.c_str());

    //
    //Provider/Requester system exists? if No -> save system into DBase
    //TABLE: system_
    //
        std::string sProviderSystemID;
        std::string sColumns = "system_name, address, port";
        std::string sValues = "'" + oServiceRegistryEntry.sProviderSystem_SystemName + "', '" +
                         oServiceRegistryEntry.sProviderSystem_Address    + "', " +
                         oServiceRegistryEntry.sProviderSystem_Port;

        if(oServiceRegistryEntry.sProviderSystem_AuthInfo.size())
        {
            sColumns += ", authentication_info";
            sValues += ", '" + oServiceRegistryEntry.sProviderSystem_AuthInfo + "'";
        }

        checkAndInsertValues(
            "id",
            "system_",
            "system_name",
            oServiceRegistryEntry.sProviderSystem_SystemName,
            sColumns,
            sValues,
            sProviderSystemID);

        printf("sProviderSystemID: %s\n", sProviderSystemID.c_str());

    //
    //Interface exists? if No -> save interface into DBase
    //TABLE: service_interface
    //
        std::vector<std::string> vInterfaceIDs;

        for(int i = 0; i < oServiceRegistryEntry.vInterfaces.size(); ++i)
        {
            std::string sID;
            checkAndInsertValue("id", "service_interface", "interface_name", oServiceRegistryEntry.vInterfaces[i], sID);
            printf("IntName: %s, ID: %s\n", oServiceRegistryEntry.vInterfaces[i].c_str(), sID.c_str());
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

        checkAndInsertValues(
            "id",
            "service_registry",
            "service_uri",
            oServiceRegistryEntry.sServiceUri,
            sColumns,
            sValues,
            sServiceRegistryEntryID
        );

        printf("ServiceRegistry ID: %s\n\n\n", sServiceRegistryEntryID.c_str());

    //
    //Save interface connection
    //TABLE: service_registry_interface_connection
    //
        for(uint i = 0; i < vInterfaceIDs.size(); ++i)
        {
            std::string sServiceRegistryIntfConnectionEntryID;

            sColumns = "service_registry_id, interface_id";
            sValues = sServiceRegistryEntryID + ", " + vInterfaceIDs[i];

            checkAndInsertValues(
                "id",
                "service_registry_interface_connection",
                "service_registry_id",
                sServiceRegistryEntryID,
                sColumns,
                sValues,
                sServiceRegistryIntfConnectionEntryID
            );
        }

//todo: create json response from database response

        auto db = Core<DBPool>::database();
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
            return "Error: Empty response from service_definition table";
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
            return "Error: Empty response from system_ table";
        }

        for(uint i = 0; i < vInterfaceIDs.size(); ++i)
        {
            sQuery = "SELECT * FROM service_interface where id = " + vInterfaceIDs[i] + ";";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                string s;
                oServiceRegistryEntry.sQData.vInterfaces_id.push_back(vInterfaceIDs[i]);
                row->get(1, s);
                oServiceRegistryEntry.sQData.vInterfaces_interfaceName.push_back(s);
                row->get(2, s);
                oServiceRegistryEntry.sQData.vInterfaces_createdAt.push_back(s);
                row->get(3, s);
                oServiceRegistryEntry.sQData.vInterfaces_updatedAt.push_back(s);
            }
            else
            {
                return "Error: Empty response from service_interface table";
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
            return "Error: Empty response from service_registry table";
        }

        return oServiceRegistryEntry.createRegistryEntry();
    }

    std::string processUnregister(const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName)
    {
        // Removes a registered service. A provider is allowed to unregister only its own services.
        // It means that provider system name and certificate common name must match for successful unregistration.
        //
        if (_pszAddr == NULL || _pszPort == NULL || _pszServDef == NULL || _pszSystemName == NULL)
            return "Error: Missing parameter(s)";

        //todo: Check provider system_name and certificate common name!
        //if SSL enabled, check ...

        auto db = Core<DBPool>::database();
        std::string sServiceDefinitionID, sProviderSystemID, sServiceRegistryId, sResponse;

        std::string sQuery = "SELECT * FROM service_definition WHERE service_definition = '" + (std::string)_pszServDef + "' ;";

        printf("Query: %s\n", sQuery.c_str());
        db.fetch(sQuery.c_str(), sServiceDefinitionID);
        printf("Answer sServiceDefinitionID: %s\n", sServiceDefinitionID.c_str());

        if(sServiceDefinitionID.size() == 0) return "ERROR: Missing information from service_definition table!";

        sQuery = "SELECT * FROM system_ WHERE system_name = '" + (std::string)_pszSystemName + "' AND " +
                  "address = '" + (std::string)_pszAddr + "' AND " +
                  "port = '" + (std::string)_pszPort + "';";

        printf("Query: %s\n", sQuery.c_str());
        db.fetch(sQuery.c_str(), sProviderSystemID);
        printf("Answer sProviderSystemID: %s\n", sProviderSystemID.c_str());

        if(sProviderSystemID.size() == 0) return "ERROR: Missing information from system_ table!";

        sQuery = "SELECT * FROM service_registry WHERE service_id = " + sServiceDefinitionID + " AND "+
                 "system_id = " + sProviderSystemID + ";";

        printf("Query: %s\n", sQuery.c_str());
        db.fetch(sQuery.c_str(), sServiceRegistryId);
        printf("Answer sServiceRegistryId: %s\n", sServiceRegistryId.c_str());

        if(sServiceRegistryId.size() == 0) return "ERROR: Missing information from service_registry table!";

        sQuery = "DELETE FROM service_registry_interface_connection WHERE service_registry_id = '" + sServiceRegistryId + "';";
        printf("Query: %s\n", sQuery.c_str());
        db.query(sQuery.c_str());

        sQuery = "DELETE FROM service_registry WHERE service_id = " + sServiceDefinitionID + " AND "+
                 "system_id = " + sProviderSystemID + ";";

        printf("Query: %s\n", sQuery.c_str());

        db.query(sQuery.c_str());

        return "OK";
    }

// Process HTTP Methods

    std::string processGETRequest(const char *_pszPage, const char *_pszItemPerPage, const char *_pszSortField, const char *_pszDirection)
    {
        // /echo	                            GET	-	                OK
        // /query/system/{id}	                GET	ID	                System
        // /mgmt/	                            GET	-	                ServiceRegistryEntryList
        // /mgmt/{id}	                        GET	ServiceID	        ServiceRegistryEntry
        // /mgmt/grouped	                    GET	-	                ServiceRegistryGrouped
        // /mgmt/servicedef/{serviceDefinition}	GET	ServiceDefinition	ServiceRegistryEntryList
        // /mgmt/services	                    GET	-	                ServiceDefinitionList
        // /mgmt/services/{id}	                GET	ServiceID	        ServiceDefinition
        // /mgmt/systems	                    GET	-	                SystemList
        // /mgmt/systems/{id}               	GET	SystemID	        System

        switch(viSubPath[0])
        {
            case ECHO:
                return "Got it";

            case QUERY:
                if(viSubPath[1] != SYSTEM) return "Unknown subpath";
                if(vsSubPath.size() < 3) return "Missing parameter (ID)";
                return processQuerySystemId(vsSubPath[2]);

            case MGMT:
                if(vsSubPath.size() == 1) return processMgmtGet();

                switch(viSubPath[1])
                {
                    case GROUPED:
                        break;

                    case SERVICEDEF:
                        if(vsSubPath.size() != 3) break;
                        return processMgmtGetServiceDef(vsSubPath[2], _pszPage, _pszItemPerPage, _pszSortField, _pszDirection);

                    case SERVICES:
                        switch(vsSubPath.size())
                        {
                            case 2: return processMgmtGetServices();
                            case 3: return processMgmtGetServicesId(vsSubPath[2]);
                            default: break;
                        }

                    case SYSTEMS:
                        switch(vsSubPath.size())
                        {
                            case 2: return processMgmtGetSystems();
                            case 3: return processMgmtGetSystemsId(vsSubPath[2]);
                            default: break;
                        }

                    default:
                        if(vsSubPath.size() != 2) break;
                        return processMgmtGetId(vsSubPath[1]);
                }

                break;
        }

        return "Unknown subpath";
    }

    std::string processPOSTRequest(const char *_szPayload)
    {
        // /query	        POST	ServiceQueryForm	    ServiceQueryList
        // /register	    POST	ServiceRegistryEntry	ServiceRegistryEntry
        // /query/system	POST	System	                System
        // /mgmt/	        POST	ServiceRegistryEntry	ServiceRegistryEntry
        // /mgmt/services	POST	ServiceDefinition	    ServiceDefinition
        // /mgmt/systems	POST	System	                System

        switch(viSubPath[0])
        {
            case QUERY:
                if(vsSubPath.size() == 1) // /query
                    return processQuery(_szPayload);
                else
                    return processQuerySystem(_szPayload);
                break;

            case REGISTER:
                if(vsSubPath.size() == 1) // /register
                    return processRegister(_szPayload);
                break;

            case MGMT:
                if(vsSubPath.size() == 1) return processMgmtPost(_szPayload);

                switch(viSubPath[1])
                {
                    case SERVICES:
                        return processMgmtPostServices(_szPayload);

                    case SYSTEMS:
                        return processMgmtPostSystems(_szPayload);
                        break;
                }

                break;
        }

        return "Unknown subpath";
    }

    std::string processPUTRequest(const char *_szPayload, bool _bCheckMandatoryFields)
    {
        // /mgmt/{id}	        PUT	ServiceRegistryEntry	ServiceRegistryEntry
        // /mgmt/services/(id}	PUT	Service	                ServiceDefinition
        // /mgmt/systems/{id}	PUT	System	                System

        switch(viSubPath[0])
        {
            case MGMT:
                if(vsSubPath.size() == 2) return processMgmtPutOrPatch(vsSubPath[1], _szPayload, _bCheckMandatoryFields);

                switch(viSubPath[1])
                {
                    case SERVICES:
                        return processMgmtPutOrPatchServices(vsSubPath[2], _szPayload, _bCheckMandatoryFields);

                    case SYSTEMS:
                        return processMgmtPutOrPatchSystems(vsSubPath[2], _szPayload, _bCheckMandatoryFields);
                        break;
                }

                break;
        }

        return "Unknown subpath!";
    }

    std::string processPATCHRequest(const char *_szPayload, bool _bCheckMandatoryFields)
    {
        // /mgmt/{id}	        PATCH	Key value pairs of ServiceRegistryEntry	ServiceRegistryEntry
        // /mgmt/services/{id}	PATCH	Key value pairs of ServiceDefinition	ServiceDefinition
        // /mgmt/systems/{id}	PATCH	Key value pairs of System	            System

        return processPUTRequest(_szPayload, _bCheckMandatoryFields);
    }

    std::string processDELETERequest(const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName)
    {
        // /unregister	        Address, Port, Service Definition, System Name in query parameters	OK
        // /mgmt/{id}	        DELETE ServiceRegistryEntryID	                                    -
        // /mgmt/services/{id}	DELETE ServiceID	                                                -
        // /mgmt/systems/{id}	DELETE SystemID	                                                    -

        switch(viSubPath[0])
        {
            case UNREGISTER:
                return processUnregister(_pszAddr, _pszPort, _pszServDef, _pszSystemName);

            case MGMT:
                if(vsSubPath.size() < 2)
                    break;

                switch(viSubPath[1])
                {
                    case SERVICES:
                        if(vsSubPath.size() < 3)
                            break;
                        printf("processMgmtDeleteServicesId\n");
                        return processMgmtDeleteServicesId(vsSubPath[2]);

                    case SYSTEMS:
                        if(vsSubPath.size() < 3)
                            break;
                        printf("processMgmtDeleteSystemsId\n");
                        return processMgmtDeleteSystemsId(vsSubPath[2]);

                    default:
                        printf("default: processMgmtDeleteId\n");
                        return processMgmtDeleteId(vsSubPath[1]);
                }

                break;
        }

        return "Unknown subpath";
    }

////////////////////////////////////////
// MHD_Callbacks
///////////////////////////////////////

    int GETCallback(const char *_szUrl, std::string &response, const char *_pszPage, const char *_pszItemPerPage, const char *_pszSortField, const char *_pszDirection)
    {
        printf("\nGET received (URL: %s)\n", _szUrl);

        if(strstr(_szUrl, "serviceregistry") == NULL)
        {
            response = "Unknown URL";
            return 0; //close TCP connection without answer
            //return 1; //response with "Unknown URL"
        }

        parseURL(_szUrl + strlen("/serviceregistry"));
        //printParsedURL();
        response = processGETRequest(_pszPage, _pszItemPerPage, _pszSortField, _pszDirection);
        return 1;
    }

    int POSTCallback  (const char *_szUrl, std::string &response, const char *payload)
    {
        printf("\nPOST received (URL: %s)\n", _szUrl);

        if(strstr(_szUrl, "serviceregistry") == NULL)
        {
            response = "Unknown URL";
            return 0; //close TCP connection without answer
            //return 1; //response with "Unknown URL"
        }

        parseURL(_szUrl + strlen("/serviceregistry"));
        response = processPOSTRequest(payload);
        return 1;
    }

    int PUTCallback (const char *_szUrl, std::string &response, const char *payload)
    {
        printf("PUT received\n");
        if(strstr(_szUrl, "serviceregistry") == NULL)
        {
            response = "Unknown URL";
            return 0; //close TCP connection without answer
            //return 1; //response with "Unknown URL"
        }

        parseURL(_szUrl + strlen("/serviceregistry"));
        response = processPUTRequest(payload, true);
        return 1;
    }

    int PATCHCallback (const char *_szUrl, std::string &response, const char *payload)
    {
        printf("PATCH received\n");
        if(strstr(_szUrl, "serviceregistry") == NULL)
        {
            response = "Unknown URL";
            return 0; //close TCP connection without answer
            //return 1; //response with "Unknown URL"
        }

        parseURL(_szUrl + strlen("/serviceregistry"));
        response = processPATCHRequest(payload, false);
        return 1;
    }

    int DELETECallback(const char *_szUrl, std::string &response, const char *addr, const char *port, const char *servdef, const char *sysname)
    {
        printf("\nDELETE received (URL: %s)\n", _szUrl);
        if(strstr(_szUrl, "serviceregistry") == NULL)
        {
            response = "Unknown URL";
            return 0; //close TCP connection without answer
            //return 1; //response with "Unknown URL"
        }

        parseURL(_szUrl + strlen("/serviceregistry"));
        response = processDELETERequest(addr, port, servdef, sysname);
        return 1;
    }

};

#endif  /*_ARROWHEAD_SERVICEREGISTRY_H_*/
