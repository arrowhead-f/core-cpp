#ifndef _CORE_CORE_H_
#define _CORE_CORE_H_


#include <cstring>
#include <string>
#include <string_view>

#include "utils/logger.h"

#include "db/DB.h"

#include "CoreUtils.h"
#include "Dispatcher.h"


template<typename DBPool, typename RB>class Core : public Dispatcher {
    private:

        DBPool &dbPool;      ///< The database pool to be used.
        RB     &reqBuilder;  ///< Used to build and send requests.

    protected:

        auto database() {
            return db::DatabaseConnection<typename DBPool::DatabaseType>{ dbPool };
        };

    public:

        Core(DBPool &dbPool, RB &reqBuilder) : dbPool{ dbPool }, reqBuilder{ reqBuilder } { }

        Response dispatch(Request &&req) final {

            (info{ } << fmt("{}: {} {}") << req.remote_address << req.method << req.uri).log(SOURCE_LOCATION);

            if (req.method == "GET") {
                return handleGET(std::move(req));
            }
            else if (req.method == "DELETE") {
                return handleDELETE(std::move(req));
            }
            else if (req.method == "POST") {
                return handlePOST(std::move(req));
            }
            else if (req.method == "PUT") {
                return handlePUT(std::move(req));
            }
            else if (req.method == "PATCH") {
                return handlePATCH(std::move(req));
            }

            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

        void error(const std::string &from, const std::string &reason) final {
            (::error{ } << fmt("{}: error; reason: {}") << from << reason.c_str()).log(SOURCE_LOCATION);
        }

        // HTTP callbacks
        virtual Response handleGET   (Request &&req) = 0;
        virtual Response handleDELETE(Request &&req) = 0;
        virtual Response handlePOST  (Request &&req) = 0;
        virtual Response handlePUT   (Request &&req) = 0;
        virtual Response handlePATCH (Request &&req) = 0;

};


#endif  /* _CORE_CORE_H_ */
