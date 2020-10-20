#ifndef _ARROWHEAD_CORE_H_
#define _ARROWHEAD_CORE_H_

#include <string>

#include "db/DB.h"
#include "net/ReqBuilder.h"

template<typename DBPool>class Core {
    private:

        DBPool &dbPool;          ///< The database pool to be used.
        ReqBuilder &reqBuilder;  ///< Used to build and send requests.

    protected:

        auto database() {
            return db::DatabaseConnection<typename DBPool::DatabaseType>{ dbPool };
        };

    public:

        Core(DBPool &dbPool, ReqBuilder &reqBuilder) : dbPool{ dbPool }, reqBuilder{ reqBuilder } { }

        // HTTP callbacks
        virtual int GETCallback   (const char *url, std::string &response, const char *page, const char *itemPerPage, const char *sortField, const char *direction) = 0;
        virtual int DELETECallback(const char *url, std::string &response, const char *addr, const char *port, const char *servdef, const char *sysname) = 0;
        virtual int POSTCallback  (const char *url, std::string &response, const char *payload) = 0;
        virtual int PUTCallback   (const char *url, std::string &response, const char *payload) = 0;
        virtual int PATCHCallback (const char *url, std::string &response, const char *payload) = 0;

};  // class Core

#endif  /*_ARROWHEAD_CORE_H_*/
