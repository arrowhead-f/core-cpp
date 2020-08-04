#pragma once

#include <stdio.h>
#include <inttypes.h>

#include "../../http/MhttpdHandler.hpp"
#include "../../json/ServiceQueryForm.hpp"
#include "../../json/ServiceQueryList.hpp"
#include "../../json/ServiceRegistryEntry.hpp"

using namespace std;

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

class ServiceRegistry : MhttpdHandler
{
private:
    vector<string> vsSubPath;
    vector<int> viSubPath;

public:
    bool startHTTPServer(uint16_t _uPort, bool _bUseHTTPS);
    void setKeyPath(string _sPubCertPath, string _sPrivKeyPath, string _sCaCertPath, string _sPasswdForKey);
    void parseURL(const char *_szURL);
    void printParsedURL();

//LOGIC
    string processQuery(const char *_szPayload);
    string processRegister(const char *_szPayload);
    string processUnregister(const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName);

    string processGETRequest();
    string processPOSTRequest(const char *_szPayload);
    string processDELETERequest(const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName);
    string processPUTRequest(const char *_szPayload);
    string processPATCHRequest(const char *_szPayload);

//MHD_Callbacks
    int GETCallback   (const char *_szUrl, string *_psResponse);
    int DELETECallback(const char *_szUrl, string *_psResponse, const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName);
    int POSTCallback  (const char *_szUrl, string *_psResponse, const char *_szPayload);
    int PUTCallback   (const char *_szUrl, string *_psResponse, const char *_szPayload);
    int PATCHCallback (const char *_szUrl, string *_psResponse, const char *_szPayload);

//Curl Callback
    size_t httpResponseCallback(char *ptr, size_t size);
};
