

#include "ServiceRegistry.hpp"

string subUrls[MAX_SUPPORTED_PATHS] = {
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

void ServiceRegistry::parseURL(const char *_szURL)
{
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

void ServiceRegistry::printParsedURL()
{
    for(int i=0; i<vsSubPath.size();++i)
        printf("%d - %s\n",i,vsSubPath[i].c_str());

    printf("\n");

    for(int i=0; i<viSubPath.size(); ++i)
        printf("%d - %d\n", i, viSubPath[i]);
}

bool ServiceRegistry::startHTTPServer(uint16_t _uPort, bool _bUseHTTPS)
{
    return !MakeServer(_uPort, _bUseHTTPS);
}

void ServiceRegistry::setKeyPath(string _sPubCertPath, string _sPrivKeyPath, string _sCaCertPath, string _sPasswdForKey)
{
    setKeys(_sPubCertPath, _sPrivKeyPath, _sCaCertPath, _sPasswdForKey);
}

////////////////////////////////////////
// LOGIC
///////////////////////////////////////

string ServiceRegistry::processGETRequest()
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

//Returns ServiceQueryList that fits the input specification.
string ServiceRegistry::processQuery(const char *_szPayload)
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


string ServiceRegistry::processRegister(const char *_szPayload)
{
    bool bSuccessfullyParsed = false;
    ServiceRegistryEntry oServiceRegistryEntry(_szPayload, bSuccessfullyParsed);

    if(!bSuccessfullyParsed)
        return "Error: Bad Json format!";

    if(!oServiceRegistryEntry.parseRegistryEntry())
        return "Error: Invalid ServiceRegistryEntry!";

//todo: DatabaseQuery
    //string sDBResponse = db.fetch(query);
    //INSERT ...

//Service definition exists? if No -> save service Definition into DBase

//Requester system exists? if No -> save system into DBase

//Interface exists? if No -> save interface into DBase

//Save service registry entry into DBase

//todo: create json response from database response
    oServiceRegistryEntry.fillResponse(); //todo: implement

    return oServiceRegistryEntry.createRegistryEntry();
}

string ServiceRegistry::processUnregister(const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName)
{
//todo: delete from database based on the arguments
    if (_pszAddr == NULL || _pszPort == NULL || _pszServDef == NULL || _pszSystemName == NULL)
        return "Error: Missing parameter";

    return "OK";
}

string ServiceRegistry::processPOSTRequest(const char *_szPayload)
{
    // /query	POST	ServiceQueryForm	ServiceQueryList
    // /register	POST	ServiceRegistryEntry	ServiceRegistryEntry
    // /query/system	POST	System	System
    // /mgmt/	POST	ServiceRegistryEntry	ServiceRegistryEntry
    // /mgmt/services	POST	ServiceDefinition	ServiceDefinition
    // /mgmt/systems	POST	System	System

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
            //todo DB
            return "OK - todo: implement";
            break;
    }

    return "Unknown subpath";
}

string ServiceRegistry::processDELETERequest(const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName)
{
    // /unregister	Address, Port, Service Definition, System Name in query parameters	OK
    // /mgmt/{id}	DELETE	ServiceRegistryEntryID	-
    // /mgmt/services/{id}	DELETE	ServiceID	-
    // /mgmt/systems/{id}	DELETE	SystemID	-

    switch(viSubPath[0])
    {
        case UNREGISTER:
            if(vsSubPath.size() == 1) // /unregister
                return processUnregister(_pszAddr, _pszPort, _pszServDef, _pszSystemName);
            break;

        case MGMT:
            //todo: DB
            return "OK - todo: implement";
            break;
    }

    return "Unknown subpath";
}

string ServiceRegistry::processPUTRequest(const char *_szPayload)
{
    // /mgmt/{id}	PUT	ServiceRegistryEntry	ServiceRegistryEntry
    // /mgmt/services/(id}	PUT	Service	ServiceDefinition
    // /mgmt/systems/{id}	PUT	System	System

    if(viSubPath[0] != MGMT)
        return "Unknown subpath";

    return "OK - todo: implement";
}

string ServiceRegistry::processPATCHRequest(const char *_szPayload)
{
    // /mgmt/{id}	PATCH	Key value pairs of ServiceRegistryEntry	ServiceRegistryEntry
    // /mgmt/services/{id}	PATCH	Key value pairs of ServiceDefinition	ServiceDefinition
    // /mgmt/systems/{id}	PATCH	Key value pairs of System	System

    if(viSubPath[0] != MGMT)
        return "Unknown subpath";

    return "OK - todo: implement";
}

////////////////////////////////////////
// MHD_Callbacks
///////////////////////////////////////

int ServiceRegistry::GETCallback(const char *_szUrl, string *_psResponse)
{
    printf("\nGET received (URL: %s)\n", _szUrl);

    if(strstr(_szUrl, "serviceregistry") == NULL)
    {
        *_psResponse = "Unknown URL";
        return 0; //close TCP connection without answer
        //return 1; //response with "Unknown URL"
    }

    parseURL(_szUrl + strlen("/serviceregistry"));
    //printParsedURL();
    *_psResponse = processGETRequest();
    return 1;
}

int ServiceRegistry::POSTCallback(const char *_szUrl, string *_psResponse, const char *_szPayload)
{
    printf("\nPOST received (URL: %s)\n", _szUrl);

    if(strstr(_szUrl, "serviceregistry") == NULL)
    {
        *_psResponse = "Unknown URL";
        return 0; //close TCP connection without answer
        //return 1; //response with "Unknown URL"
    }

    parseURL(_szUrl + strlen("/serviceregistry"));
    *_psResponse = processPOSTRequest(_szPayload);
    return 1;
}

int ServiceRegistry::DELETECallback(const char *_szUrl, string *_psResponse, const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName)
{
    printf("\nDELETE received (URL: %s)\n", _szUrl);
    if(strstr(_szUrl, "serviceregistry") == NULL)
    {
        *_psResponse = "Unknown URL";
        return 0; //close TCP connection without answer
        //return 1; //response with "Unknown URL"
    }

    parseURL(_szUrl + strlen("/serviceregistry"));
    *_psResponse = processDELETERequest(_pszAddr, _pszPort, _pszServDef, _pszSystemName);
    return 1;
}

int ServiceRegistry::PUTCallback(const char *_szUrl, string *_psResponse, const char *_szPayload)
{
    printf("PUT received\n");
    if(strstr(_szUrl, "serviceregistry") == NULL)
    {
        *_psResponse = "Unknown URL";
        return 0; //close TCP connection without answer
        //return 1; //response with "Unknown URL"
    }

    parseURL(_szUrl + strlen("/serviceregistry"));
    *_psResponse = processPUTRequest(_szPayload);
    return 1;
}

int ServiceRegistry::PATCHCallback(const char *_szUrl, string *_psResponse, const char *_szPayload)
{
    printf("PATCH received\n");
    if(strstr(_szUrl, "serviceregistry") == NULL)
    {
        *_psResponse = "Unknown URL";
        return 0; //close TCP connection without answer
        //return 1; //response with "Unknown URL"
    }

    parseURL(_szUrl + strlen("/serviceregistry"));
    *_psResponse = processPATCHRequest(_szPayload);
    return 1;
}

////////////////////////////////////////
// CURL callback
///////////////////////////////////////

size_t ServiceRegistry::httpResponseCallback(char *ptr, size_t size)
{
    printf("HTTP Response Callback\n");
    printf("HTTP content:\n %s\n", ptr);

	return size;
}
