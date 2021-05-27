
#ifndef _ENDPOINTS_MGMTGET_H_
#define _ENDPOINTS_MGMTGET_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "core/Core.h"
#include "../utils/DbWrapper.h"
#include "../utils/Error.h"
#include "../payloads/SRSystem.h"

template<typename DB>
class MgmtGet {

    private:
        DB &db;

    public:

        MgmtGet(DB &db) : db{ db } {}

        Response processMgmtGetSystemsId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/systems/{id}"}.getResp();

            SRSystem oSRSystem;
            uint8_t status = processSystem(_Id, oSRSystem);

            if(status)
                return ErrorResp{"System with id " + std::to_string(_Id) + " not found.", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems/{id}"}.getResp();

            return Response{ oSRSystem.createSRSystem() };
        }

        uint8_t processSystem(int _Id, SRSystem &_roSystem)
        {
            std::string sQuery = "SELECT * FROM system_ WHERE id = '" + std::to_string(_Id) + "';";
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                std::string s;
                row->get(0, _roSystem.sId);
                row->get(1, _roSystem.sSystemName);
                row->get(2, _roSystem.sAddress);
                row->get(3, _roSystem.sPort);
                row->get(4, _roSystem.sAuthInfo);
                row->get(5, _roSystem.sCreatedAt);
                row->get(6, _roSystem.sUpdatedAt);
                return 0;
            }
            return 1;
        }

        Response processMgmtGetServicesId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();
            return Response{ "in progress" };
        }

        Response processMgmtGetId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
            return Response{ "in progress" };
        }
};

#endif   /* _ENDPOINTS_MGMTGET_H_ */
