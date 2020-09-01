#ifndef _ARROWHEAD_SERVICEREGISTRY_H_
#define _ARROWHEAD_SERVICEREGISTRY_H_

#include "core/Core.h"

#include <cstring>

#include "../../json/ServiceQueryForm.hpp"
#include "../../json/ServiceQueryList.hpp"
#include "../../json/ServiceRegistryEntry.hpp"

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

    std::string processQuery(const char *_szPayload)
    {
        bool bSuccessfullyParsed = false;
        ServiceQueryForm oServiceQueryForm(_szPayload, bSuccessfullyParsed);

        if(!bSuccessfullyParsed)
            return "Error: Bad Json format!";

        if(!oServiceQueryForm.parseQueryForm())
            return "Error: Invalid ServiceQueryForm!";

    //todo: DatabaseQuery, logikai hierarchia??
        string dbQuery = "Select * from * where " + oServiceQueryForm.sServiceDefinition + " ";

    //Filter on interfaces
        for(int i = 0; i < oServiceQueryForm.vInterfaceRequirements.size(); ++i)
            dbQuery += oServiceQueryForm.vInterfaceRequirements[i] + " ";

    //Filter on Security type
        for(int i = 0; i < oServiceQueryForm.vSecurityRequirements.size(); ++i)
            dbQuery += oServiceQueryForm.vSecurityRequirements[i] + " ";

    //Filter on Metadata
        for (auto it = oServiceQueryForm.mMetadataRequirements.begin(); it != oServiceQueryForm.mMetadataRequirements.end(); ++it)
            dbQuery += it->first + ":" + it->second + " ";

    //Filter on Version
        if(oServiceQueryForm.sVersionReq.length())    dbQuery += oServiceQueryForm.sVersionReq + " ";
        if(oServiceQueryForm.sMinVersionReq.length()) dbQuery += oServiceQueryForm.sMinVersionReq + " ";
        if(oServiceQueryForm.sMaxVersionReq.length()) dbQuery += oServiceQueryForm.sMaxVersionReq + " ";

    //Filter on Ping ???
        if(oServiceQueryForm.sPingProviders.length()) dbQuery += oServiceQueryForm.sPingProviders + " ";

    //todo: DBQuery
        printf("DBQuery: %s\n", dbQuery.c_str());

    //todo: fill response content from DB answer

        ServiceQueryList oServiceQueryList; //this will be the answer Json content
        oServiceQueryList.fillContent();
        return oServiceQueryList.createServiceQueryList();
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

    //todo: create json response from database response
        printf("\n\tCreate json response from database response\n");

        auto db = Core<DBPool>::database();
        oServiceRegistryEntry.sQData.sId = sServiceRegistryEntryID;

        std::string sQuery = "SELECT * FROM service_definition where id = " + sServiceDefinitionID + ";";
        printf("Query: %s\n", sQuery.c_str());
        std::string resp;
        db.fetch(sQuery.c_str(), resp);
        printf("resp: %s\n", resp.c_str());

        if(auto row = db.fetch(sQuery.c_str())) {
            printf("visszaadott\n");
            int id;
            row->get(0, id);
            printf("id: %d\n", id);
        }
        else{
            printf("nem adott vissza semmit \n");
        }

        //auto row = db.fetch(sQuery.c_str());
        //int iValue;
        //row->get(1, iValue);
        //printf("%d\n", iValue);
        //oServiceRegistryEntry.sQData.sServiceDefinition_id = std::to_string(iValue);
        //row->get(1, oServiceRegistryEntry.sQData.sServiceDefinition_serviceDefinition);
        //row->get(2, oServiceRegistryEntry.sQData.sServiceDefinition_createdAt);
        //row->get(3, oServiceRegistryEntry.sQData.sServiceDefinition_updatedAt);

        oServiceRegistryEntry.fillResponse(); //todo: implement

        return oServiceRegistryEntry.createRegistryEntry();
    }

    std::string processUnregister(const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName)
    {
    //todo: delete from database based on the arguments
        if (_pszAddr == NULL || _pszPort == NULL || _pszServDef == NULL || _pszSystemName == NULL)
            return "Error: Missing parameter(s)";

        return "OK - todo:implement";
    }

    std::string processGETRequest()
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
                if(vsSubPath.size() < 3) return "Missing ID";
                //printf("system id: %s\n", vsSubPath[2].c_str());
                //todo: DataBaseQuery by ID, return system
                return "OK - todo: implement";
                break;

            case MGMT:
                return "OK - todo: implement";
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
                break;
            case REGISTER:
                if(vsSubPath.size() == 1) // /register
                    return processRegister(_szPayload);
                break;
            case MGMT:
                return "OK - todo: implement";
                break;
        }

        return "Unknown subpath";
    }

    std::string processPUTRequest(const char *_szPayload)
    {
        // /mgmt/{id}	        PUT	ServiceRegistryEntry	ServiceRegistryEntry
        // /mgmt/services/(id}	PUT	Service	                ServiceDefinition
        // /mgmt/systems/{id}	PUT	System	                System

        if(viSubPath[0] != MGMT)
            return "Unknown subpath";

        return "OK - todo: implement";
    }

    std::string processPATCHRequest(const char *_szPayload)
    {
        // /mgmt/{id}	        PATCH	Key value pairs of ServiceRegistryEntry	ServiceRegistryEntry
        // /mgmt/services/{id}	PATCH	Key value pairs of ServiceDefinition	ServiceDefinition
        // /mgmt/systems/{id}	PATCH	Key value pairs of System	            System

        if(viSubPath[0] != MGMT)
            return "Unknown subpath";

        return "OK - todo: implement";
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
                if(vsSubPath.size() == 1) // /unregister
                    return processUnregister(_pszAddr, _pszPort, _pszServDef, _pszSystemName);
                break;

            case MGMT:
                return "OK - todo: implement";
                break;
        }

        return "Unknown subpath";
    }

////////////////////////////////////////
// MHD_Callbacks
///////////////////////////////////////

    int GETCallback(const char *_szUrl, std::string &response)
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
        response = processGETRequest();
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
        response = processPUTRequest(payload);
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
        response = processPATCHRequest(payload);
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
