#ifndef _ARROWHEAD_SERVICEREGISTRY_H_
#define _ARROWHEAD_SERVICEREGISTRY_H_

#include "core/Core.h"

#include <iostream>  /* for testing purposes */


template<typename DBPool>class ServiceRegistry final : public Core<DBPool> {
    private:

    public:

        using Core<DBPool>::Core;

        int GETCallback(const char *url, std::string &response) {
            std::cout << "get\n";
            return 0;
        }

        int DELETECallback(const char *url, std::string &response, const char *addr, const char *port, const char *servdef, const char *sysname) {
            std::cout << "delete\n";
            return 0;
        }

        int POSTCallback  (const char *url, std::string &response, const char *payload) {
            std::cout << "post\n";
            return 0;
        }

        int PUTCallback   (const char *url, std::string &response, const char *payload) {
            std::cout << "pput\n";
            return 0;
        }

        int PATCHCallback (const char *url, std::string &response, const char *payload) {
            std::cout << "patch\n";
            return 0;
        }

};

#endif  /*_ARROWHEAD_SERVICEREGISTRY_H_*/
