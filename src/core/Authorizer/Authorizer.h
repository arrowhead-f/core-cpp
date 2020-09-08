#ifndef _ARROWHEAD_AUTHORIZER_H_
#define _ARROWHEAD_AUTHORIZER_H_

#include "core/Core.h"

template<typename DBPool>class Authorizer final : public Core<DBPool> {

public:
    using Core<DBPool>::Core;

    int GETCallback(const char *_szUrl, std::string &response)
    {
        return 1;
    }

    int POSTCallback  (const char *_szUrl, std::string &response, const char *payload)
    {
        return 1;
    }

    int PUTCallback (const char *_szUrl, std::string &response, const char *payload)
    {
        return 1;
    }

    int PATCHCallback (const char *_szUrl, std::string &response, const char *payload)
    {
        return 1;
    }

    int DELETECallback(const char *_szUrl, std::string &response, const char *addr, const char *port, const char *servdef, const char *sysname)
    {
        return 1;
    }

};

#endif  /*_ARROWHEAD_AUTHORIZER_H_*/
