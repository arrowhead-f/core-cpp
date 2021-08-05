#ifndef _CORE_SERVICEREGISTRY_H_
#define _CORE_SERVICEREGISTRY_H_

#include "core/Core.h"

#include "endpoints/Register.h"
#include "endpoints/SRQuery.h"
#include "endpoints/MgmtDelete.h"
#include "endpoints/MgmtGet.h"
#include "endpoints/MgmtPost.h"
#include "endpoints/MgmtPut.h"
#include "endpoints/MgmtPatch.h"

#include "http/crate/Uri.h"

template<typename DBPool, typename RB>class ServiceRegistry final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

    private:


    public:

        using Core<DBPool, RB>::Core;

        Response handleGET(Request &&req) final
        {
            if( req.uri.compare("/echo") )
            {
                return Response{ "Got it!" };
            }

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

            if (req.uri.compare("/mgmt/grouped"))
            {
                auto db = Parent::database();
                return MgmtGet<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtGetGrouped();
            }

            if( req.uri.compare("/mgmt/systems") )
            {
                int page = 0;
                int itemPerPage = 0x7FffFFff;
                std::string sortField;
                std::string direction;

                auto parser = Uri::Parser{ req.uri };

                if ( parser.check() == true && static_cast<bool>(parser) == true )
                {
                    while(1)
                    {
                        auto &&kv = *parser;
                        if ( kv.first.compare("page") == 0 )
                        {
                            page = std::stoi(kv.second);
                        }
                        else if ( kv.first.compare("item_per_page") == 0 )
                        {
                            itemPerPage = std::stoi(kv.second);
                        }
                        else if ( kv.first.compare("sort_field") == 0 )
                        {
                            sortField = kv.second;
                        }
                        else if ( kv.first.compare("direction") == 0 )
                        {
                            direction = kv.second;
                        }

                        ++parser;

                        if ( static_cast<bool>(parser) == false )
                            break;
                    }
                }

                auto db = Parent::database();
                return MgmtGet<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtGetSystems(page, itemPerPage, sortField, direction);
            }

            if( req.uri.consume("/mgmt/systems") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtGet<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtGetSystemsId( id );
                }
            }

            if( req.uri.compare("/mgmt/services") )
            {
                int page = 0;
                int itemPerPage = 0x7FffFFff;
                std::string sortField;
                std::string direction;

                auto parser = Uri::Parser{ req.uri };

                if ( parser.check() == true && static_cast<bool>(parser) == true )
                {
                    while(1)
                    {
                        auto &&kv = *parser;
                        if ( kv.first.compare("page") == 0 )
                        {
                            page = std::stoi(kv.second);
                        }
                        else if ( kv.first.compare("item_per_page") == 0 )
                        {
                            itemPerPage = std::stoi(kv.second);
                        }
                        else if ( kv.first.compare("sort_field") == 0 )
                        {
                            sortField = kv.second;
                        }
                        else if ( kv.first.compare("direction") == 0 )
                        {
                            direction = kv.second;
                        }

                        ++parser;

                        if ( static_cast<bool>(parser) == false )
                            break;
                    }
                }

                auto db = Parent::database();
                return MgmtGet<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtGetServices(page, itemPerPage, sortField, direction);
            }

            if( req.uri.consume("/mgmt/services") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtGet<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtGetServicesId( id );
                }
            }

            if( req.uri.consume("/mgmt/servicedef") )
            {
                std::string serviceDef;
                if( req.uri.pathParam(serviceDef) )
                {
                    int page = 0;
                    int itemPerPage = 0x7FffFFff;
                    std::string sortField;
                    std::string direction;

                    auto parser = Uri::Parser{ req.uri };

                    if ( parser.check() == true && static_cast<bool>(parser) == true )
                    {
                        while(1)
                        {
                            auto &&kv = *parser;
                            if ( kv.first.compare("page") == 0 )
                            {
                                page = std::stoi(kv.second);
                            }
                            else if ( kv.first.compare("item_per_page") == 0 )
                            {
                                itemPerPage = std::stoi(kv.second);
                            }
                            else if ( kv.first.compare("sort_field") == 0 )
                            {
                                sortField = kv.second;
                            }
                            else if ( kv.first.compare("direction") == 0 )
                            {
                                direction = kv.second;
                            }

                            ++parser;

                            if ( static_cast<bool>(parser) == false )
                                break;
                        }
                    }

                    auto db = Parent::database();
                    return MgmtGet<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtGetServiceDef( serviceDef, page, itemPerPage, sortField, direction );
                }
            }

            if( req.uri.consume("/mgmt") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtGet<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtGetId( id );
                }
                else
                {
                    int page = 0;
                    int itemPerPage = 0x7FffFFff;
                    std::string sortField;
                    std::string direction;

                    auto parser = Uri::Parser{ req.uri };

                    if ( parser.check() == true && static_cast<bool>(parser) == true )
                    {
                        while(1)
                        {
                            auto &&kv = *parser;
                            if ( kv.first.compare("page") == 0 )
                            {
                                page = std::stoi(kv.second);
                            }
                            else if ( kv.first.compare("item_per_page") == 0 )
                            {
                                itemPerPage = std::stoi(kv.second);
                            }
                            else if ( kv.first.compare("sort_field") == 0 )
                            {
                                sortField = kv.second;
                            }
                            else if ( kv.first.compare("direction") == 0 )
                            {
                                direction = kv.second;
                            }

                            ++parser;

                            if ( static_cast<bool>(parser) == false )
                                break;
                        }
                    }

                    auto db = Parent::database();
                    return MgmtGet<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtGet(page, itemPerPage, sortField, direction);
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
            if ( req.uri.compare("/query/system") ) {
                auto db = Parent::database();
                return SRQuery<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processQuerySystem(std::move(req));
            }
            if ( req.uri.compare("/mgmt/services") ) {
                auto db = Parent::database();
                return MgmtPost<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtPostServiceDefinition(std::move(req));
            }
            if ( req.uri.compare("/mgmt/systems") ) {
                auto db = Parent::database();
                return MgmtPost<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtPostSystems(std::move(req));
            }

            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePATCH(Request &&req) final {
            if( req.uri.consume("/mgmt/services") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtPatch<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtPatchServiceDefinition(std::move(req), std::to_string(id) );
                }
            }

            if( req.uri.consume("/mgmt/systems") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtPatch<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtPatchSystems(std::move(req), std::to_string(id) );
                }
            }

            return Response::from_stock(http::status_code::NotImplemented);
        }

        Response handlePUT(Request &&req) final {
            if( req.uri.consume("/mgmt/services") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtPut<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtPutServiceDefinition(std::move(req), std::to_string(id) );
                }
            }

            if( req.uri.consume("/mgmt/systems") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtPut<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtPutSystems(std::move(req), std::to_string(id) );
                }
            }

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

            if( req.uri.consume("/mgmt/systems") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtDelete<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtDeleteSystemsId( id );
                }
            }

            if( req.uri.consume("/mgmt/services") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtDelete<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtDeleteServicesId( id );
                }
            }

            if( req.uri.consume("/mgmt") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    return MgmtDelete<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processMgmtDeleteId( id );
                }
            }

            return Response::from_stock(http::status_code::NotImplemented);
        }


};

#endif  /* _CORE_SERVICEREGISTRY_H_ */
