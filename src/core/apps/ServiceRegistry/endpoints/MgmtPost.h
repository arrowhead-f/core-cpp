
#ifndef _ENDPOINTS_MGMTPOST_H_
#define _ENDPOINTS_MGMTPOST_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "core/Core.h"
#include "utils/common/DbWrapper.h"
#include "utils/common/Error.h"

#include "utils/common/CommonPayloads.h"

template<typename DB>
class MgmtPost : CommonPayloads {

    private:
        DB &db;

    public:

        MgmtPost(DB &db) : db{ db } {}

        Response processMgmtPostSystems(Request &&req)
        {
            SRSystem oSRSystem;
            if(!oSRSystem.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/systems"}.getResp();

            uint8_t status = oSRSystem.validSystem();

            switch(status)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5: return ErrorResp{"parameter null or empty", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems"}.getResp();
                case 6: return ErrorResp{"Port must be between 0 and 65535.", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems"}.getResp();
            }

            std::string sProviderSystemID;
            std::string sColumns = "system_name, address, port";
            std::string sValues = "'" +
                                    oSRSystem.stSystemData.sSystemName + "', '" +
                                    oSRSystem.stSystemData.sAddress    + "', " +
                                    oSRSystem.stSystemData.sPort;

            if(oSRSystem.stSystemData.sAuthInfo.size())
            {
                sColumns += ", authentication_info";
                sValues += ", '" + oSRSystem.stSystemData.sAuthInfo + "'";
            }

            DbWrapper<DB> dbw(db);

            dbw.checkAndInsertValues(
                "id",
                "system_",
                "system_name",
                oSRSystem.stSystemData.sSystemName,
                sColumns,
                sValues,
                sProviderSystemID);

            //printf("sProviderSystemID: %s\n", sProviderSystemID.c_str());

            std::string sQuery = "SELECT * FROM system_ where system_name = '" + oSRSystem.stSystemData.sSystemName +
                                 "' AND address = '" + oSRSystem.stSystemData.sAddress +
                                 "' AND port = '"    + oSRSystem.stSystemData.sPort    + "';";

            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, oSRSystem.stSystemData.sId);
                row->get(4, oSRSystem.stSystemData.sAuthInfo);
                row->get(5, oSRSystem.stSystemData.sCreatedAt);
                row->get(6, oSRSystem.stSystemData.sUpdatedAt);
            }
            else
            {
                return ErrorResp{"No system with name: " + oSRSystem.stSystemData.sSystemName + ", address: " + oSRSystem.stSystemData.sAddress + " and port: " + oSRSystem.stSystemData.sPort, 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems"}.getResp();
            }

            return Response{ oSRSystem.createSRSystem() };
        }

        Response processMgmtPostServiceDefinition(Request &&req)
        {
            ServiceDefinition oServiceDefinition;
            if(!oServiceDefinition.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services"}.getResp();

            uint8_t status = oServiceDefinition.validJSONPayload();
            switch( status )
            {
                case 1: return ErrorResp{"Service definition is not specified.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services"}.getResp();
                case 2: return ErrorResp{"Service definition is null or blank.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services"}.getResp();
                case 10: return ErrorResp{"JSon parsing exception.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services"}.getResp();
            }

            std::string sServiceDefinitionID;
            DbWrapper<DB> dbw(db);
            dbw.checkAndInsertValue("id", "service_definition", "service_definition", oServiceDefinition.stServDefData.sServiceDefinition, sServiceDefinitionID);

            std::string sQuery = "SELECT * FROM service_definition WHERE id = '" + sServiceDefinitionID + "';";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, oServiceDefinition.stServDefData.sId);
                row->get(1, oServiceDefinition.stServDefData.sServiceDefinition);
                row->get(2, oServiceDefinition.stServDefData.sCreatedAt);
                row->get(3, oServiceDefinition.stServDefData.sUpdatedAt);
                return Response{ oServiceDefinition.createServiceDefinition() };
            }

            return ErrorResp{"Service definition with id of '" + sServiceDefinitionID + "' does not exist", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/services"}.getResp();
        }
};

#endif   /* _ENDPOINTS_MGMTPOST_H_ */
