#ifndef _CORE_SERVICEREGISTRY_H_
#define _CORE_SERVICEREGISTRY_H_

#include "core/Core.h"

#include "endpoints/Register.h"
#include "endpoints/SRQuery.h"

#include "http/crate/Uri.h"

template<typename DBPool, typename RB>class ServiceRegistry final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

    private:


    public:

        using Core<DBPool, RB>::Core;

        Response handleGET(Request &&req) final {
            if( req.uri.compare("/echo") ){ return Response{"Got it!"}; }

            if( req.uri.consume("/query/system") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return SRQuery<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processQuerySystemId( id );
                }
                else
                {
                    return Response::from_stock(http::status_code::NotFound);
                }
            }

            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePOST(Request &&req) final {
            if ( req.uri.compare("/register") ) {
                auto db = Parent::database();
                return Register<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processRegister(std::move(req));
            }
            if ( req.uri.compare("/query") ) {
                auto db = Parent::database();
                return SRQuery<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processQuery(std::move(req));
            }

            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePATCH(Request &&req) final {
            return Response::from_stock(http::status_code::NotImplemented);
        }

        Response handlePUT(Request &&req) final {
            return Response::from_stock(http::status_code::NotImplemented);
        }

        Response handleDELETE(Request &&req) final {
            auto parser = Uri::Parser{ req.uri };

            if ( parser.check() == true && static_cast<bool>(parser) == true )
            {
                if ( req.uri.compare("/unregister") )
                {
                    std::string service_definition;
                    std::string system_name;
                    std::string address;
                    std::string port;

                    bool servDefExists = false;
                    bool sysNameExists = false;
                    bool addrExists = false;
                    bool portExists = false;

                    while(1){
                        auto &&kv = *parser;
                        if ( kv.first.compare("service_definition") == 0 )
                        {
                            servDefExists = true;
                            service_definition = kv.second;
                        }
                        else if ( kv.first.compare("system_name") == 0 )
                        {
                            sysNameExists = true;
                            system_name = kv.second;
                        }
                        else if ( kv.first.compare("address") == 0 )
                        {
                            addrExists = true;
                            address = kv.second;
                        }
                        else if ( kv.first.compare("port") == 0 )
                        {
                            portExists = true;
                            port = kv.second;
                        }

                        ++parser;

                        if ( static_cast<bool>(parser) == false )
                            break;
                    }

                    if ( servDefExists && sysNameExists && addrExists && portExists )
                    {
                        auto db = Parent::database();
                        return Register<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processUnregister(service_definition, system_name, address, port);
                    }
                    else
                    {
                        return Response::from_stock(http::status_code::BadRequest);
                    }
                }
            }

            return Response::from_stock(http::status_code::NotImplemented);
        }
};

#endif  /* _CORE_SERVICEREGISTRY_H_ */
