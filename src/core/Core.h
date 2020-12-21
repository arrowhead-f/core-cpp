#ifndef _ARROWHEAD_CORE_H_
#define _ARROWHEAD_CORE_H_

#include <cstring>
#include <string>

#include "utils/logger.h"

#include "db/DB.h"
#include "Dispatcher.h"
#include "ErrCodes.h"
#include "http/ReqBuilder.h"

template<typename DBPool, typename RB>class Core : public Dispatcher {
    private:

        DBPool &dbPool;          ///< The database pool to be used.
        RB     &reqBuilder;  ///< Used to build and send requests.

    protected:

        auto database() {
            return db::DatabaseConnection<typename DBPool::DatabaseType>{ dbPool };
        };

    public:

        Core(DBPool &dbPool, RB &reqBuilder) : dbPool{ dbPool }, reqBuilder{ reqBuilder } { }

        int dispatch(const std::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) final {

            (info{ } << fmt("{}: {} {}") << from << method << uri).log(SOURCE_LOCATION);

            if(!std::strcmp(method, "GET")) {
                return handleGET(uri, response);
            }
            else if(!std::strcmp(method, "DELETE")) {
                return handleDELETE(uri, response, payload);
            }
            else if(!std::strcmp(method, "POST")) {
                return handlePOST(uri, response, payload);
            }
            else if(!std::strcmp(method, "PUT")) {
                return handlePUT(uri, response, payload);
            }
            else if(!std::strcmp(method, "PATCH")) {
                return handlePATCH(uri, response, payload);
            }

            return ErrCode::UNKNOWN_METHOD;
        }

        void error(const std::string &from, const char *method, const std::string &uri) final {
            (warning{ } << fmt("{}: {} {}") << from << method << uri).log(SOURCE_LOCATION);
        }

        // HTTP callbacks
        virtual int handleGET   (const std::string &uri, std::string &response) = 0;
        virtual int handleDELETE(const std::string &uri, std::string &response, const std::string &payload) = 0;
        virtual int handlePOST  (const std::string &uri, std::string &response, const std::string &payload) = 0;
        virtual int handlePUT   (const std::string &uri, std::string &response, const std::string &payload) = 0;
        virtual int handlePATCH (const std::string &uri, std::string &response, const std::string &payload) = 0;

};  // class Core

#endif  /*_ARROWHEAD_CORE_H_*/