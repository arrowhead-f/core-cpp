

#include "MhttpdHandler.hpp"

#define POSTBUFFERSIZE  2048
#define MAXNAMESIZE      20
#define MAXANSWERSIZE   2048
#define POST              1

struct connection_info_struct
{
    int connectiontype;
    char *answerstring;
    char *szResponse;
    int iRespLength;
    struct MHD_PostProcessor *postprocessor;
};

size_t MhttpdHandler::httpResponseCallback(char *ptr, size_t size)
{
	return size;
}

size_t httpResponseHandler(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	return ((MhttpdHandler *)userdata)->httpResponseCallback(ptr, size*nmemb);
}

int MhttpdHandler::SendRequest(string _sPayload, string _sURL, string _sMethod, bool _bUseHTTPS)
{
    int http_code = 0;
    CURLcode res;
    CURL *curl;
    struct curl_slist *headers = NULL;
    string agent;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

  	if(curl)
  	{
		agent = "libcurl/"+string(curl_version_info(CURLVERSION_NOW)->version);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str());

		headers = curl_slist_append(headers, "Expect:");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

//if (strcmp(pmethod.c_str(), "PUT") == 0)
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, _sMethod.c_str());
//else if( strcmp( pmethod.c_str(), "POST") == 0)
//      curl_easy_setopt(curl, CURLOPT_POST, true);

        if(_bUseHTTPS)
        {
            //---------------HTTPS SECTION--------------------------------------------------------
            //--verbose
            //if ( curl_easy_setopt(curl, CURLOPT_VERBOSE,        1L)            != CURLE_OK)
            //   printf("error: CURLOPT_VERBOSE\n");
            //--insecure
            if ( curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L)            != CURLE_OK)
                 printf("error: CURLOPT_SSL_VERIFYPEER\n");
            if ( curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L)            != CURLE_OK)
                 printf("error: CURLOPT_SSL_VERIFYHOST\n");
            //--cert
            if ( curl_easy_setopt(curl, CURLOPT_SSLCERT,        sPubCertPath.c_str() /*"keys/tempsensor.testcloud1.publicCert.pem"*/)  != CURLE_OK)
                 printf("error: CURLOPT_SSLCERT\n");
            //--cert-type
            if ( curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE,    "PEM")         != CURLE_OK)
                 printf("error: CURLOPT_SSLCERTTYPE\n");
            //--key
            if ( curl_easy_setopt(curl, CURLOPT_SSLKEY,         sPrivKeyPath.c_str() /*"keys/tempsensor.testcloud1.private.key"*/) != CURLE_OK)
                 printf("error: CURLOPT_SSLKEY\n");
            //--key-type
            if ( curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE,     "PEM")         != CURLE_OK)
                 printf("error: CURLOPT_SSLKEYTYPE\n");
            //--pass
            if ( curl_easy_setopt(curl, CURLOPT_KEYPASSWD,      sPasswdForKey.c_str() /*"12345"*/)       != CURLE_OK)
                 printf("error: CURLOPT_KEYPASSWD\n");
            //--cacert
            if ( curl_easy_setopt(curl, CURLOPT_CAINFO,         sCaCertPath.c_str() /*"keys/tempsensor.testcloud1.caCert.pem"*/)  != CURLE_OK)
                 printf("error: CURLOPT_CAINFO\n");
            //
            //---------------END OF HTTPS SECTION-------------------------------------------------
        }

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _sPayload.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &httpResponseHandler);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

		curl_easy_setopt(curl, CURLOPT_URL, _sURL.c_str());

		// Perform the request, res will get the return code
		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
            printf("Error: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
  	}

    curl_global_cleanup();
    return http_code;
}

// SERVER side
int MhttpdHandler::GETCallback(const char *_szReceivedURL, string *_sResponse)
{
    *_sResponse = "1234";
    return 1;
}

int MhttpdHandler::POSTCallback(const char *_szUrl, string *_psResponse, const char *_szPayload)
{
    *_psResponse = "1234";
     return 1;
}

int MhttpdHandler::DELETECallback(const char *url, string *_sResponse, const char *_pszAddr, const char *_pszPort, const char *_pszServDef, const char *_pszSystemName)
{
    *_sResponse = "1234";
    return 1;
}

int MhttpdHandler::PUTCallback(const char *_szUrl, string *_psResponse, const char *_szPayload)
{
    *_psResponse = "1234";
     return 1;
}

int MhttpdHandler::PATCHCallback(const char *_szUrl, string *_psResponse, const char *_szPayload)
{
    *_psResponse = "1234";
     return 1;
}

inline int checkMethodName(const char *method)
{
    if( strcmp(method, MHD_HTTP_METHOD_GET) == 0) return METHOD_GET;
    if( strcmp(method, MHD_HTTP_METHOD_POST) == 0) return METHOD_POST;
    if( strcmp(method, MHD_HTTP_METHOD_PUT) == 0) return METHOD_PUT;
    if( strcmp(method, MHD_HTTP_METHOD_PATCH) == 0) return METHOD_PATCH;
    if( strcmp(method, MHD_HTTP_METHOD_DELETE) == 0) return METHOD_DELETE;

    return METHOD_UNKNOWN;
}

inline int iterate_post(void *coninfo_cls,
                        enum MHD_ValueKind kind,
                        const char *key,
                        const char *filename,
                        const char *content_type,
                        const char *transfer_encoding,
                        const char *data,
                        uint64_t off,
                        size_t size)
{
    struct connection_info_struct *con_info = (connection_info_struct *) coninfo_cls;

    if (0 == strcmp(key, "name"))
    {
        if ((size > 0) && (size <= MAXNAMESIZE))
        {
            char *answerstring;
            answerstring = (char *) malloc(MAXANSWERSIZE);
            if (!answerstring)
                return MHD_NO;

            snprintf(answerstring, MAXANSWERSIZE, "{\"name\": %s}", data);
            con_info->answerstring = answerstring;
        }
        else
        {
            con_info->answerstring = NULL;
        }

        return MHD_NO;
    }

    return MHD_YES;
}

const char szContentType[] = "application/x-www-form-urlencoded";

// static C function (not member of class!)
extern "C" int MHD_Callback(void *cls,
          struct MHD_Connection *connection,
          const char *url,
          const char *method,
          const char *version,
          const char *upload_data,
          size_t *upload_data_size,
          void **con_cls)
{
    static int aptr;
    struct MHD_Response *stMHDResponse;

    int callbackResponse = 0;
    int ret = MHD_YES;
    int iMethod = checkMethodName(method);
    string sResponse;

    switch(iMethod)
    {
        case METHOD_GET:
        case METHOD_DELETE:
        {
            // do never respond on first call
            if (&aptr != *con_cls)
            {
                *con_cls = &aptr;
                return MHD_YES;
            }
            *con_cls = NULL;  // reset when done

            if(iMethod == METHOD_DELETE)
            {
                const char *addr        = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "Address");
                const char *port        = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "Port");
                const char *servDef     = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "ServiceDefinition");
                const char *systemName  = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "SystemName");

                callbackResponse = ((MhttpdHandler *)cls)->DELETECallback(url, &sResponse, addr, port, servDef, systemName);
            }
            else
            {
                callbackResponse = ((MhttpdHandler *)cls)->GETCallback(url, &sResponse);
            }

            if (callbackResponse)
            {
                stMHDResponse = MHD_create_response_from_buffer(sResponse.length(), (void *)sResponse.c_str(), MHD_RESPMEM_MUST_COPY);
                MHD_add_response_header (stMHDResponse, "Content-Type", "application/json");
                ret = MHD_queue_response(connection, MHD_HTTP_OK, stMHDResponse);
                MHD_destroy_response(stMHDResponse);
            }

            break;
        }
        case METHOD_POST:
        case METHOD_PUT:
        case METHOD_PATCH:
        {
            MHD_replace_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_TYPE, (char *)szContentType);

            if (*con_cls == NULL)
            {
                struct connection_info_struct *con_info = (connection_info_struct *)malloc(sizeof(struct connection_info_struct));

                if (con_info == NULL)
                    return MHD_NO;

                con_info->answerstring = NULL;
                con_info->szResponse = NULL;
                con_info->iRespLength = 0;
                con_info->postprocessor = MHD_create_post_processor(connection, 2048, iterate_post, (void *)con_info);

                if (con_info->postprocessor == NULL)
                {
                    free(con_info);
                    return MHD_NO;
                }

                *con_cls = (void *)con_info;

                return MHD_YES;
            }

            struct connection_info_struct *con_info = (connection_info_struct *)*con_cls;

            if (*upload_data_size != 0)
            {
                MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);

                switch(iMethod)
                {
                    case METHOD_POST:
                        ((MhttpdHandler *)cls)->POSTCallback(url, &sResponse, upload_data);
                        break;
                    case METHOD_PUT:
                        ((MhttpdHandler *)cls)->PUTCallback(url, &sResponse, upload_data);
                        break;
                    case METHOD_PATCH:
                        ((MhttpdHandler *)cls)->PATCHCallback(url, &sResponse, upload_data);
                        break;
                }

                con_info->szResponse = (char *)malloc(sResponse.length() + 1);
                strcpy(con_info->szResponse, sResponse.c_str());
                con_info->iRespLength = sResponse.length();

                *upload_data_size = 0;

                return MHD_YES;
            }
            else
            {
                stMHDResponse = MHD_create_response_from_buffer(con_info->iRespLength, (void *)con_info->szResponse, MHD_RESPMEM_MUST_COPY);
                free(con_info->szResponse);
                MHD_add_response_header (stMHDResponse, "Content-Type", "application/json");
                ret = MHD_queue_response(connection, MHD_HTTP_OK, stMHDResponse);
                MHD_destroy_response(stMHDResponse);
            }
            break;
        }
        default:
        {
            string sResp = "\"Result\":\"Unknown method\"";
            stMHDResponse = MHD_create_response_from_buffer(sResp.length(), (void *)sResp.c_str(), MHD_RESPMEM_MUST_COPY);
            MHD_add_response_header (stMHDResponse, "Content-Type", "application/json");
            ret = MHD_queue_response(connection, MHD_HTTP_OK, stMHDResponse);
            MHD_destroy_response(stMHDResponse);
        }
    }

    return ret;
}

void MhttpdHandler::setKeys(string _sPubCertPath, string _sPrivKeyPath, string _sCaCertPath, string _sPasswdForKey)
{
    sPubCertPath  = _sPubCertPath;
    sPrivKeyPath  = _sPrivKeyPath;
    sCaCertPath   = _sCaCertPath;
    sPasswdForKey = _sPasswdForKey;
}

void request_completed(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe)
{
    struct connection_info_struct *con_info = (connection_info_struct *) *con_cls;

    if (NULL == con_info)
        return;

    MHD_destroy_post_processor(con_info->postprocessor);
    if (con_info->answerstring)
        free(con_info->answerstring);

    free(con_info);
    *con_cls = NULL;
}

static long get_file_size (const char *filename)
{
    FILE *fp;

    fp = fopen (filename, "rb");
    if (fp)
    {
        long size;

        if ((0 != fseek (fp, 0, SEEK_END)) || (-1 == (size = ftell (fp))))
            size = 0;

        fclose (fp);

        return size;
    }
    else
    {
        return 0;
    }
}

static char *load_file (const char *filename)
{
    FILE *fp;
    char *buffer;
    long size;

    size = get_file_size (filename);
    if (size == 0)
        return NULL;

    fp = fopen (filename, "rb");
    if (!fp)
        return NULL;

    buffer = (char *)malloc (size);
    if (!buffer)
    {
        fclose (fp);
        return NULL;
    }

    if (size != fread (buffer, 1, size, fp))
    {
        free (buffer);
        buffer = NULL;
    }

    fclose (fp);
    return buffer;
}

int MhttpdHandler::MakeServer(unsigned short _uListenPort, bool _bUseHTTPS)
{
    if(_bUseHTTPS)
    {
        bUsingHTTPS = true;
        pszPrivKey  = load_file(sPrivKeyPath.c_str()); //password protected private key   key_pem
        pszPubCert  = load_file(sPubCertPath.c_str()); //cert_pem
        pszCaCert   = load_file(sCaCertPath.c_str()); //root_ca_pem

        if ((pszPrivKey == NULL) || (pszPubCert == NULL) || (pszCaCert == NULL))
        {
            printf ("The key/certificate files could not be read.\n");
            return 1;
        }

        pmhd = MHD_start_daemon(
			MHD_USE_THREAD_PER_CONNECTION |  MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_SSL | MHD_USE_DEBUG | MHD_USE_DUAL_STACK, //one thread per connection
			_uListenPort,
			NULL,
            NULL,
            &MHD_Callback,
            this,
//Additional arguments
//1
            MHD_OPTION_NOTIFY_COMPLETED, //Register a function that should be called whenever a request has been completed
            request_completed,
            NULL,
//2
            MHD_OPTION_CONNECTION_TIMEOUT, //After how many seconds of inactivity should a connection automatically be timed out?
            (unsigned int) 120,
//3
			MHD_OPTION_HTTPS_MEM_KEY,
            pszPrivKey,
//4
			MHD_OPTION_HTTPS_KEY_PASSWORD,
            (char *)sPasswdForKey.c_str(),
//5
            MHD_OPTION_HTTPS_MEM_CERT,
            pszPubCert,
//6
            MHD_OPTION_HTTPS_MEM_TRUST,
            pszCaCert,

			MHD_OPTION_END);
    }
    else
    {
        pmhd = MHD_start_daemon(
                MHD_USE_THREAD_PER_CONNECTION |  MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_DEBUG | MHD_USE_DUAL_STACK, //one thread per connection
    			//MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG | MHD_USE_DUAL_STACK, //Run using an internal thread doing SELECT.
    			_uListenPort,
    			NULL,
    			NULL,
    			&MHD_Callback, //default handler for all URIs
    			this, //extra argument for the MhttpdHandler
    //Additional arguments are a list of options (type-value pairs, terminated with MHD_OPTION_END). It is mandatory to use MHD_OPTION_END as last argument, even when there are no additional arguments.
    //1
                MHD_OPTION_NOTIFY_COMPLETED, //Register a function that should be called whenever a request has been completed
    			request_completed,
    			NULL,
    //2
    			MHD_OPTION_CONNECTION_TIMEOUT, //After how many seconds of inactivity should a connection automatically be timed out?
    			(unsigned int) 120,
    //3
                //MHD_OPTION_THREAD_POOL_SIZE, //Number (unsigned int) of threads in thread pool.
                //500,
    //4
    //            MHD_OPTION_CONNECTION_LIMIT, //Maximum number of concurrent connections to accept
    //            10000,
    			MHD_OPTION_END);
    }

	if (pmhd == NULL)
    {
        //printf("MakeServer error\n");
        return 1;
    }

    //printf("MakeServer success\n");

	return 0;
}

int MhttpdHandler::KillServer()
{
	if (pmhd)
	{
		MHD_stop_daemon(pmhd);
		pmhd = NULL;

        if (bUsingHTTPS)
        {
            free (pszPrivKey);
		    free (pszPubCert);
            free(pszCaCert);
        }

		return 0;
	}

    return 1;
}
