
#ifndef _ENDPOINTS_MGMTPATCH_H_
#define _ENDPOINTS_MGMTPATCH_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "core/Core.h"
#include "utils/common/DbWrapper.h"
#include "utils/common/Error.h"

#include "utils/common/CommonPayloads.h"

template<typename DB>
class MgmtPatch : CommonPayloads {

    private:
        DB &db;

    public:

        MgmtPatch(DB &db) : db{ db } {}

        Response processMgmtPatchSystems(Request &&req, std::string _sId)
        {
            SRSystem oSRSystem;
            if(!oSRSystem.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/systems/{id}"}.getResp();

            if(!oSRSystem.validSystem2())
            {
                return ErrorResp{"Port must be between 0 and 65535.", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems/{id}"}.getResp();
            }

            std::string sQuery;
            if(oSRSystem.stSystemData.sSystemName.size())
            {
                sQuery = "UPDATE system_ SET system_name = '" + oSRSystem.stSystemData.sSystemName + "'";
                sQuery += " WHERE id = '" + _sId + "';";
                db.query(sQuery.c_str());
            }

            if(oSRSystem.stSystemData.sAddress.size())
            {
                sQuery = "UPDATE system_ SET address = '" + oSRSystem.stSystemData.sAddress + "'";
                sQuery += " WHERE id = '" + _sId + "';";
                db.query(sQuery.c_str());
            }

            if(oSRSystem.stSystemData.sPort.size())
            {
                sQuery = "UPDATE system_ SET port = '" + oSRSystem.stSystemData.sPort + "'";
                sQuery += " WHERE id = '" + _sId + "';";
                db.query(sQuery.c_str());
            }

            if(oSRSystem.stSystemData.sAuthInfo.size())
            {
                sQuery = "UPDATE system_ SET authentication_info = '" + oSRSystem.stSystemData.sAuthInfo + "'";
                sQuery += " WHERE id = '" + _sId + "';";
                db.query(sQuery.c_str());
            }

            sQuery = "SELECT * FROM system_ WHERE id = '" + _sId + "'";

            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, oSRSystem.stSystemData.sId);
                row->get(1, oSRSystem.stSystemData.sSystemName);
                row->get(2, oSRSystem.stSystemData.sAddress);
                row->get(3, oSRSystem.stSystemData.sPort);
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

        Response processMgmtPatchServiceDefinition(Request &&req, std::string _sId)
        {
            ServiceDefinition oServiceDefinition;
            if(!oServiceDefinition.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();

            uint8_t status = oServiceDefinition.validJSONPayload();

            std::string sQuery;
            if(status == 0)
            {
                sQuery = "UPDATE service_definition SET service_definition = '" + oServiceDefinition.stServDefData.sServiceDefinition + "' WHERE id = '" + _sId + "';";
                db.query(sQuery.c_str());
            }

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

#endif   /* _ENDPOINTS_MGMTPATCH_H_ */
