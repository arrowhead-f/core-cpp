#ifndef _CORE_ORCHESTRATOR_H_
#define _CORE_ORCHESTRATOR_H_


#include "core/Core.h"
#include "core/helpers/InvokeIf.h"

#include "endpoints/Orchestration.h"

template<typename DBPool, typename RB>class Orchestrator final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

    private:

        Response urlEcho(Request &&req) {
            return Response::from_stock(http::status_code::OK);
        }

    public:

        using Core<DBPool, RB>::Core;

        Response handleGET(Request &&req) final {
            if( req.uri.compare("/echo") )
            {
                return Response{ "Got it!" };
            }

            if( req.uri.consume("/orchestration") )
            {
                int id;
                if( req.uri.pathId(id) )
                {
                    auto db = Parent::database();
                    auto requestBuilder = Parent::reqBuilder;

                    return Orchestration<db::DatabaseConnection<typename DBPool::DatabaseType>, RB>{ db, requestBuilder }.processOrchestrationId( id, "/orchestrator/orchestration/{id}" );
                }
            }

            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePOST(Request &&req) final {
            if ( req.uri.compare("/orchestration")) {
                auto db = Parent::database();
                auto requestBuilder = Parent::reqBuilder;
                return Orchestration<db::DatabaseConnection<typename DBPool::DatabaseType>, RB>{ db, requestBuilder }.processOrchestration(std::move(req), "/orchestrator/orchestration");
            }

            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_ORCHESTRATOR_H_ */
