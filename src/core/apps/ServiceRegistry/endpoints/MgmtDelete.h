
#ifndef _ENDPOINTS_MGMTDELETE_H_
#define _ENDPOINTS_MGMTDELETE_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "core/Core.h"
#include "../utils/DbWrapper.h"
#include "../utils/Error.h"

template<typename DB>
class MgmtDelete {

    private:
        DB &db;
        ServiceRegistryEntry oServiceRegistryEntry;

    public:

        MgmtDelete(DB &db) : db{ db } {}

        Response processMgmtDeleteSystemsId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/systems/{id}"}.getResp();

            std::string sQuery = "DELETE FROM system_ WHERE id = '" + std::to_string(_Id) + "';";
            db.query(sQuery.c_str());
            return Response{ "OK" };
        }

        Response processMgmtDeleteServicesId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();

            std::string sQuery = "DELETE FROM service_definition WHERE id = '" + std::to_string(_Id) + "';";
            db.query(sQuery.c_str());
            return Response{ "OK" };
        }

        Response processMgmtDeleteId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();

            std::string sQuery = "DELETE FROM service_registry WHERE id = '" + std::to_string(_Id) + "';";
            db.query(sQuery.c_str());
            return Response{ "OK" };
        }
};

#endif   /* _ENDPOINTS_MGMTDELETE_H_ */
