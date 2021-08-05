
#ifndef _ENDPOINTS_MGMTPUT_H_
#define _ENDPOINTS_MGMTPUT_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "core/Core.h"
#include "../utils/DbWrapper.h"
#include "../utils/Error.h"

#include "../payloads/SRPayloads.h"

template<typename DB>
class MgmtPut : SRPayloads {

    private:
        DB &db;

    public:

        MgmtPut(DB &db) : db{ db } {}

        Response processMgmtPutSystems(Request &&req, std::string _sId)
        {
            SRSystem oSRSystem;
            if(!oSRSystem.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/systems/{id}"}.getResp();

            uint8_t status = oSRSystem.validSystem();

            switch(status)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5: return ErrorResp{"parameter null or empty", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems/{id}"}.getResp();
                case 6: return ErrorResp{"Port must be between 0 and 65535.", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems/{id}"}.getResp();
            }

            std::string sQuery = "UPDATE system_ SET system_name = '" + oSRSystem.stSystemData.sSystemName + "', " +
                                 "address = '" + oSRSystem.stSystemData.sAddress + "', " +
                                 "port = " + oSRSystem.stSystemData.sPort;// + "";

            if(oSRSystem.stSystemData.sAuthInfo.size())
            {
                sQuery += ", authentication_info = '" + oSRSystem.stSystemData.sAuthInfo + "'";
            }

            sQuery += " WHERE id = '" + _sId + "';";

            db.query(sQuery.c_str());

            sQuery = "SELECT * FROM system_ where id = '" + _sId + "'";

            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, oSRSystem.stSystemData.sId);
                row->get(4, oSRSystem.stSystemData.sAuthInfo);
                row->get(5, oSRSystem.stSystemData.sCreatedAt);
                row->get(6, oSRSystem.stSystemData.sUpdatedAt);
            }
            else
            {
                return ErrorResp{"No system with name: " + oSRSystem.stSystemData.sSystemName + ", address: " + oSRSystem.stSystemData.sAddress + " and port: " + oSRSystem.stSystemData.sPort, 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems/{id}"}.getResp();
            }

            return Response{ oSRSystem.createSRSystem() };

        }

        Response processMgmtPutServiceDefinition(Request &&req, std::string _sId)
        {
            ServiceDefinition oServiceDefinition;
            if(!oServiceDefinition.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();

            uint8_t status = oServiceDefinition.validJSONPayload();
            switch( status )
            {
                case 1: return ErrorResp{"Service definition is not specified.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();
                case 2: return ErrorResp{"Service definition is null or blank.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();
                case 10: return ErrorResp{"JSon parsing exception.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();
            }

            std::string sQuery = "UPDATE service_definition SET service_definition = '" + oServiceDefinition.stServDefData.sServiceDefinition + "' WHERE id = '" + _sId + "';";

            db.query(sQuery.c_str());

            sQuery = "SELECT * from service_definition WHERE id = '" + _sId + "'";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, oServiceDefinition.stServDefData.sId);
                row->get(1, oServiceDefinition.stServDefData.sServiceDefinition);
                row->get(2, oServiceDefinition.stServDefData.sCreatedAt);
                row->get(3, oServiceDefinition.stServDefData.sUpdatedAt);
                return Response{ oServiceDefinition.createServiceDefinition() };
            }

            return ErrorResp{"Service definition with id of '" + _sId + "' does not exist", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/services/{id}"}.getResp();
        }
};

#endif   /* _ENDPOINTS_MGMTPUT_H_ */
