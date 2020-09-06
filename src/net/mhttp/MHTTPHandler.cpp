#include "MHTTPHandler.h"

#include <cstring>

#define METHOD_GET       1
#define METHOD_POST      2
#define METHOD_DELETE    3
#define METHOD_PUT       4
#define METHOD_PATCH     5
#define METHOD_UNKNOWN 999


struct connection_info_struct {
    int connectiontype;
    char *answerstring;
    char *szResponse;
    int iRespLength;
    MHD_PostProcessor *postprocessor;
};

int checkMethodName(const char *method) {
    if(!strcmp(method, MHD_HTTP_METHOD_GET))    return METHOD_GET;
    if(!strcmp(method, MHD_HTTP_METHOD_POST))   return METHOD_POST;
    if(!strcmp(method, MHD_HTTP_METHOD_PUT))    return METHOD_PUT;
    if(!strcmp(method, MHD_HTTP_METHOD_PATCH))  return METHOD_PATCH;
    if(!strcmp(method, MHD_HTTP_METHOD_DELETE)) return METHOD_DELETE;

    return METHOD_UNKNOWN;
}


#define POSTBUFFERSIZE  2048
#define MAXNAMESIZE      20
#define MAXANSWERSIZE   2048
#define POST              1




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



namespace {

    constexpr char *szContentType = "application/x-www-form-urlencoded";

    /// Replace a particular header value. If multiple values match the kind, return any one of them.
    /// \param connection connection to get values from
    /// \param kind       what kind of value are we looking for
    /// \param key        the header to look for, NULL to lookup 'trailing' value without a key
    /// \param newVal     the new value of the key
    /// \return           false if no such item was found
/*
    bool MHD_ReplaceConnVal(MHD_Connection *connection, MHD_ValueKind kind, const char *key, char *newVal) {
        MHD_HTTP_Header *pos;

        if (!connection)
            return false;

        for (pos = connection->headers_received; nullptr != pos; pos = pos->next) {
            if ((0 != (pos->kind & kind)) &&  ( (key == pos->header) || ( (nullptr != pos->header) && (nullptr != key) && (MHD_str_equal_caseless_(key, pos->header))))) {
                pos->value = newVal;
                return true;
            }
        }
        return false;
    }
*/
}



void MHD_ReqComplete(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe) {
    struct connection_info_struct *con_info = (connection_info_struct *) *con_cls;

    if (NULL == con_info)
        return;

    MHD_destroy_post_processor(con_info->postprocessor);
    if (con_info->answerstring)
        free(con_info->answerstring);

    free(con_info);
    *con_cls = NULL;
}


int MHD_ReqCallback(void *cls, MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
        static int aptr = 0;

        MHD_Response *stMHDResponse = nullptr;

        int callbackResponse = 0;
        int ret = MHD_YES;
        std::string sResponse;

        switch(const auto iMethod = checkMethodName(method))
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

                    // we are sure the type
                    callbackResponse = static_cast<HTTPHandlerBase*>(cls)->DELETECallback(url, sResponse, addr, port, servDef, systemName);
                }
                else
                {
                    callbackResponse = static_cast<HTTPHandlerBase*>(cls)->GETCallback(url, sResponse);
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
                //MHD_ReplaceConnVal(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_TYPE, szContentType);

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
                            static_cast<HTTPHandlerBase*>(cls)->POSTCallback(url, sResponse, upload_data);
                            break;
                        case METHOD_PUT:
                            static_cast<HTTPHandlerBase*>(cls)->PUTCallback(url, sResponse, upload_data);
                            break;
                        case METHOD_PATCH:
                            static_cast<HTTPHandlerBase*>(cls)->PATCHCallback(url, sResponse, upload_data);
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
                std::string sResp = "\"Result\":\"Unknown method\"";
                stMHDResponse = MHD_create_response_from_buffer(sResp.length(), (void *)sResp.c_str(), MHD_RESPMEM_MUST_COPY);
                MHD_add_response_header (stMHDResponse, "Content-Type", "application/json");
                ret = MHD_queue_response(connection, MHD_HTTP_OK, stMHDResponse);
                MHD_destroy_response(stMHDResponse);
            }
        }

        return ret;
    }


