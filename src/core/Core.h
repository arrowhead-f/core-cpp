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

        Response dispatch(Request &&req) noexcept final {
            (info{ } << fmt("{}: {} {}") << req.remote_address << req.method << req.uri).log(SOURCE_LOCATION);
            return handle(std::move(req));
        }

        virtual Response handle(Request &&req) {
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
            else if (req.method == "HEAD") {
                return handleHEAD(std::move(req));
            }
            else if (req.method == "OPTIONS") {
                return handleOPTIONS(std::move(req));
            }

            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

        void report(const std::string &from, const std::string &reason) final {
            (::error{ } << fmt("{}: error; reason: {}") << from << reason.c_str()).log(SOURCE_LOCATION);
        }

        void report(const std::string &from, const char *reason) final {
            (::error{ } << fmt("{}: error; reason: {}") << from << reason).log(SOURCE_LOCATION);
        }

        // HTTP callbacks
        virtual Response handleGET    (Request &&req)  {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handleDELETE (Request &&req)  {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handlePUT    (Request &&req)  {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handlePATCH  (Request &&req)  {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handlePOST   (Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handleHEAD   (Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handleOPTIONS(Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_CORE_H_ */
