#pragma once

#include <string>
#include <cstring>
#include <stdio.h>

//#include <gnutls/gnutls.h>
//#include <gnutls/x509.h>

#include "../../inc/curl/curl.h"
#include "../../inc/mhttpd/microhttpd.h" //include/microhttpd/microhttpd.h

using namespace std;

#define METHOD_GET       1
#define METHOD_POST      2
#define METHOD_DELETE    3
#define METHOD_PUT       4
#define METHOD_PATCH     5
#define METHOD_UNKNOWN 999

class MhttpdHandler
{
private:
	struct MHD_Daemon *pmhd = NULL;
	bool   bUsingHTTPS;
	string sPubCertPath;
	string sPrivKeyPath;
	string sCaCertPath;
	string sPasswdForKey;
	char  *pszPrivKey;
	char  *pszPubCert;
	char  *pszCaCert;

public:

	MhttpdHandler()
	{
		bUsingHTTPS = false;
	}

	//for HTTPS support
	MhttpdHandler(string _sPubCertPath, string _sPrivKeyPath, string _sCaCertPath, string _sPasswdForKey)
	{
		bUsingHTTPS   = true;
	    sPubCertPath  = _sPubCertPath;
	    sPrivKeyPath  = _sPrivKeyPath;
	    sCaCertPath   = _sCaCertPath;
		sPasswdForKey = _sPasswdForKey;
	}

	void setKeys(string _sPubCertPath, string _sPrivKeyPath, string _sCaCertPath, string _sPasswdForKey);

	virtual int GETCallback   (const char *_szUrl, string *_psResponse);
    virtual int DELETECallback(const char *_szUrl, string *_psResponse, const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName);
    virtual int POSTCallback  (const char *_szUrl, string *_psResponse, const char *_szPayload);
    virtual int PUTCallback   (const char *_szUrl, string *_psResponse, const char *_szPayload);
    virtual int PATCHCallback (const char *_szUrl, string *_psResponse, const char *_szPayload);

	int SendRequest(string _sPayload, string _sURL, string _sMethod, bool _bUseHTTPS);
	virtual size_t httpResponseCallback(char *ptr, size_t size);

	int MakeServer(unsigned short uListenPort, bool _bUseHTTPS);
	int KillServer();
};
