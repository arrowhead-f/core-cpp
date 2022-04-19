
#ifndef _ENDPOINTS_MGMTGET_H_
#define _ENDPOINTS_MGMTGET_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "core/Core.h"
#include "utils/common/DbWrapper.h"
#include "utils/common/Error.h"
#include "utils/common/CommonPayloads.h"
#include "../payloads/SRSystem.h"
#include "../payloads/SRSystemList.h"
#include "../payloads/ServiceRegistryEntry.h"
//#include "../payloads/ServiceRegistryEntryList.h"
#include "../payloads/ServiceDefinition.h"
#include "../payloads/ServiceDefinitionList.h"


template<typename DB>
class MgmtGet : CommonPayloads {

    private:
        DB &db;

    public:

        MgmtGet(DB &db) : db{ db } {}

        Response processMgmtGet(int page, int item_per_page, std::string sort_field, std::string direction)
        {
            std::string sQuery = "SELECT id FROM service_registry";

            if(sort_field.size() != 0)
            {
                if( sort_field.compare("id") == 0 ||  sort_field.compare("createdAt") == 0 || sort_field.compare("updatedAt") == 0 )
                {
                    sQuery += ( sort_field.compare("id") == 0 ? std::string(" ORDER BY id") :
                              ( sort_field.compare("createdAt") == 0 ? std::string(" ORDER BY created_at") :
                              ( sort_field.compare("updatedAt") == 0 ? std::string(" ORDER BY updated_at") : "")));

                    if(direction.size() != 0)
                    {
                        if( direction.compare("ASC") != 0 || direction.compare("DESC") != 0)
                        {
                            sQuery += " " + direction;
                        }
                        else
                        {
                            return ErrorResp{"Unknown parameter '" + direction + "'", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/services"}.getResp();
                        }
                    }
                }
                else
                {
                    return ErrorResp{"Sortable field with reference '" + sort_field + "' is not available", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/services"}.getResp();
                }
            }

            if(item_per_page > 0)
                sQuery += " LIMIT " + std::to_string(item_per_page);

            if(page > 0)
                sQuery += " OFFSET " + std::to_string(page);

            int count = 0;
            std::string resp = "{ \"data\" : [";
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    std::string sID;
                    row->get(0, sID);

                    ServiceRegistryEntry oServiceRegistryEntry;
                    uint8_t status = processServiceRegistryEntry(std::stoi(sID), oServiceRegistryEntry);

                    switch(status)
                    {
                        case 1: return ErrorResp{"Empty response from service_registry table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                        case 2: return ErrorResp{"Empty response from service_definition table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                        case 3: return ErrorResp{"Empty response from system_ table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                        case 4: return ErrorResp{"Empty response from service_registry_interface_connection table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                        case 5: return ErrorResp{"Empty response from service_interface table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                    }

                    resp += oServiceRegistryEntry.createRegistryEntry() + std::string(",");
                    ++count;
                } while( row->next() );
            }
            else
            {
                return ErrorResp{"Empty response from service_registry table", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/grouped"}.getResp();
            }

            resp.back() = ']';
            resp += ", \"count\" : " + std::to_string(count) + "}";

            return Response{ resp };
        }

        //Returns all Service Registry Entries grouped
        //todo: fill else cases
        Response processMgmtGetGrouped()
        {
            std::string resp = "{ \"autoCompleteData\" : { \"interfaceList\" : [";
            std::string sQuery = "SELECT id,interface_name FROM service_interface";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    std::string sID;
                    std::string sIntfName;
                    row->get(0, sID);
                    row->get(1, sIntfName);
                    resp += "{\"id\" : " + sID + ", \"value\" : \"" + sIntfName + "\"},";
                } while( row->next() );
            }
            resp.back() = ']';
            resp += ",\"serviceList\" : [";

            sQuery = "SELECT id,service_definition FROM service_definition";
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    std::string sID;
                    std::string sServDef;
                    row->get(0, sID);
                    row->get(1, sServDef);
                    resp += "{\"id\" : " + sID + ", \"value\" : \"" + sServDef + "\"},";
                } while( row->next() );
            }
            resp.back() = ']';
            resp += ",\"systemList\" : [";

            sQuery = "SELECT id FROM system_";
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    std::string sID;
                    row->get(0, sID);

                    SRSystem oSRSystem;
                    processSystem(sID, oSRSystem);
                    resp += oSRSystem.createSRSystem();
                    resp += ",";
                } while( row->next() );
            }

            resp.back() = ']';
            resp += "},";

            resp += "\"servicesGroupedByServiceDefinition\" :[";
            sQuery = "SELECT id,service_definition FROM service_definition";
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    resp += "{";
                    std::string sID;
                    std::string sServDef;
                    row->get(0, sID);
                    row->get(1, sServDef);
                    resp += "\"serviceDefinitionId\" : " + sID + ", \"serviceDefinition\" : \"" + sServDef + "\"";
                    resp += ",\"providerServices\" : [";

                    sQuery = "SELECT id FROM service_registry WHERE service_id = '" + sID + "'";

                    if (auto row = db.fetch(sQuery.c_str()) )
                    {
                        do{
                            row->get(0, sID);

                            ServiceRegistryEntry oServiceRegistryEntry;
                            uint8_t status = processServiceRegistryEntry(std::stoi(sID), oServiceRegistryEntry);

                            switch(status)
                            {
                                case 1: return ErrorResp{"Empty response from service_registry table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                                case 2: return ErrorResp{"Empty response from service_definition table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                                case 3: return ErrorResp{"Empty response from system_ table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                                case 4: return ErrorResp{"Empty response from service_registry_interface_connection table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                                case 5: return ErrorResp{"Empty response from service_interface table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                            }

                            resp += oServiceRegistryEntry.createRegistryEntry() + std::string(",");
                        } while( row->next() );
                    }
                    else
                    {
                        return ErrorResp{"Empty response from service_registry table", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/grouped"}.getResp();
                    }

                    resp.back() = ']'; //providerServices
                    resp += "},";
                } while( row->next() );
            }

            resp.back() = ']'; //servicesGroupedByServiceDefinition

            resp += ",\"servicesGroupedBySystems\" :[";

            sQuery = "SELECT id,system_name,address,port FROM system_";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    resp += "{";
                    std::string sID;
                    std::string sSysName;
                    std::string sAddr;
                    std::string sPort;
                    row->get(0, sID);
                    row->get(1, sSysName);
                    row->get(2, sAddr);
                    row->get(3, sPort);

                    resp += "\"systemId\" : " + sID + ", \"systemName\" : \"" + sSysName + "\"" + ", \"address\" : \"" + sAddr + "\", \"port\" : " + sPort;
                    resp += ",\"services\" : [";

                    sQuery = "SELECT id FROM service_registry WHERE system_id = '" + sID + "'";

                    if (auto row = db.fetch(sQuery.c_str()) )
                    {
                        do{
                            row->get(0, sID);

                            ServiceRegistryEntry oServiceRegistryEntry;
                            uint8_t status = processServiceRegistryEntry(std::stoi(sID), oServiceRegistryEntry);

                            switch(status)
                            {
                                case 1: return ErrorResp{"Empty response from service_registry table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                                case 2: return ErrorResp{"Empty response from service_definition table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                                case 3: return ErrorResp{"Empty response from system_ table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                                case 4: return ErrorResp{"Empty response from service_registry_interface_connection table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                                case 5: return ErrorResp{"Empty response from service_interface table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/grouped"}.getResp();
                            }

                            resp += oServiceRegistryEntry.createRegistryEntry() + std::string(",");
                        } while( row->next() );
                    }
                    else
                    {
                        return ErrorResp{"Empty response from service_registry table", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/grouped"}.getResp();
                    }

                    resp.back() = ']'; //services
                    resp += "},";
                } while( row->next() );
            }

            resp.back() = ']'; //servicesGroupedBySystems

            resp += "}";
            return Response{ resp };
        }

        Response processMgmtGetId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();

            ServiceRegistryEntry oServiceRegistryEntry;
            uint8_t status = processServiceRegistryEntry(_Id, oServiceRegistryEntry);

            switch(status)
            {
                case 1: return ErrorResp{"Empty response from service_registry table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
                case 2: return ErrorResp{"Empty response from service_definition table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
                case 3: return ErrorResp{"Empty response from system_ table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
                case 4: return ErrorResp{"Empty response from service_registry_interface_connection table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
                case 5: return ErrorResp{"Empty response from service_interface table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/{id}"}.getResp();
            }

            return Response{ oServiceRegistryEntry.createRegistryEntry() };
        }

        uint8_t processServiceRegistryEntry(int _Id, ServiceRegistryEntry &_roServiceRegistryEntry)
        {
            std::string sServiceDefinitionID;
            std::string sProviderSystemID;
            std::vector<std::string> vsInterfaceIDs;

            std::string sQuery = "SELECT * FROM service_registry where id = '" + std::to_string(_Id) + "';";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, _roServiceRegistryEntry.sQData.sId);
                row->get(1, sServiceDefinitionID);
                row->get(2, sProviderSystemID);
                row->get(3, _roServiceRegistryEntry.sQData.sServiceUri);
                row->get(4, _roServiceRegistryEntry.sQData.sEndOfValidity);
                row->get(5, _roServiceRegistryEntry.sQData.sSecure);
                row->get(6, _roServiceRegistryEntry.sQData.sMetadata);
                row->get(7, _roServiceRegistryEntry.sQData.sVersion);
                row->get(8, _roServiceRegistryEntry.sQData.sCreatedAt);
                row->get(9, _roServiceRegistryEntry.sQData.sUpdatedAt);
            }
            else
            {
                 return 1;
            }

            sQuery = "SELECT * FROM service_definition where id = " + sServiceDefinitionID + ";";

            if ( auto row = db.fetch(sQuery.c_str()) )
            {
                _roServiceRegistryEntry.sQData.sServiceDefinition_id = sServiceDefinitionID;
                row->get(1, _roServiceRegistryEntry.sQData.sServiceDefinition_serviceDefinition);
                row->get(2, _roServiceRegistryEntry.sQData.sServiceDefinition_createdAt);
                row->get(3, _roServiceRegistryEntry.sQData.sServiceDefinition_updatedAt);
            }
            else
            {
                 return 2;
            }

            sQuery = "SELECT * FROM system_ where id = " + sProviderSystemID + ";";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                _roServiceRegistryEntry.sQData.sProvider_id = sProviderSystemID;
                row->get(1, _roServiceRegistryEntry.sQData.sProvider_systemName);
                row->get(2, _roServiceRegistryEntry.sQData.sProvider_address);
                row->get(3, _roServiceRegistryEntry.sQData.sProvider_port);
                row->get(4, _roServiceRegistryEntry.sQData.sProvider_authenticationInfo);
                row->get(5, _roServiceRegistryEntry.sQData.sProvider_createdAt);
                row->get(6, _roServiceRegistryEntry.sQData.sProvider_updatedAt);
            }
            else
            {
                return 3;
            }

            sQuery = "SELECT interface_id FROM service_registry_interface_connection WHERE service_registry_id = '" + _roServiceRegistryEntry.sQData.sId + "';";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    std::string s;
                    row->get(0, s);
                    vsInterfaceIDs.push_back(s);
                } while( row->next() );
            }
            else
            {
                return 4;
            }

            for(uint i = 0; i < vsInterfaceIDs.size(); ++i)
            {
                sQuery = "SELECT * FROM service_interface where id = " + vsInterfaceIDs[i] + ";";
                if (auto row = db.fetch(sQuery.c_str()) )
                {
                    std::string s;
                    _roServiceRegistryEntry.sQData.vInterfaces_id.push_back(vsInterfaceIDs[i]);
                    row->get(1, s);
                    _roServiceRegistryEntry.sQData.vInterfaces_interfaceName.push_back(s);
                    row->get(2, s);
                    _roServiceRegistryEntry.sQData.vInterfaces_createdAt.push_back(s);
                    row->get(3, s);
                    _roServiceRegistryEntry.sQData.vInterfaces_updatedAt.push_back(s);
                }
                else
                {
                    return 5;
                }
            }

            return 0;
        }

        Response processMgmtGetSystems(int page, int item_per_page, std::string sort_field, std::string direction)
        {
            SRSystemList oSRSystemList;
            oSRSystemList.uCount = 0;

            std::string sQuery = "SELECT id FROM system_";

            if(sort_field.size() != 0)
            {
                if( sort_field.compare("id") == 0 ||  sort_field.compare("createdAt") == 0 || sort_field.compare("updatedAt") == 0 )
                {
                    sQuery += ( sort_field.compare("id") == 0 ? std::string(" ORDER BY id") :
                              ( sort_field.compare("createdAt") == 0 ? std::string(" ORDER BY created_at") :
                              ( sort_field.compare("updatedAt") == 0 ? std::string(" ORDER BY updated_at") : "")));

                    if(direction.size() != 0)
                    {
                        if( direction.compare("ASC") != 0 || direction.compare("DESC") != 0)
                        {
                            sQuery += " " + direction;
                        }
                        else
                        {
                            return ErrorResp{"Unknown parameter '" + direction + "'", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems"}.getResp();
                        }
                    }
                }
                else
                {
                    return ErrorResp{"Sortable field with reference '" + sort_field + "' is not available", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems"}.getResp();
                }
            }

            if(item_per_page > 0)
                sQuery += " LIMIT " + std::to_string(item_per_page);

            if(page > 0)
                sQuery += " OFFSET " + std::to_string(page);

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    std::string sID;
                    row->get(0, sID);

                    SRSystem oSRSystem;
                    processSystem(sID, oSRSystem);

                    oSRSystemList.vSRSystem.push_back(oSRSystem.stSystemData);
                    oSRSystemList.uCount++;
                } while( row->next() );
            }
            else
            {
                return ErrorResp{"Empty response from system_ table", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems"}.getResp();
            }

            return Response{ oSRSystemList.createSystemList() };
        }

        Response processMgmtGetSystemsId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/systems/{id}"}.getResp();

            SRSystem oSRSystem;
            uint8_t status = processSystem(std::to_string(_Id), oSRSystem);

            if(status)
                return ErrorResp{"System with id " + std::to_string(_Id) + " not found.", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/systems/{id}"}.getResp();

            return Response{ oSRSystem.createSRSystem() };
        }

        uint8_t processSystem(std::string _sId, SRSystem &_roSystem)
        {
            std::string sQuery = "SELECT * FROM system_ WHERE id = '" + _sId + "';";
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                std::string s;
                row->get(0, _roSystem.stSystemData.sId);
                row->get(1, _roSystem.stSystemData.sSystemName);
                row->get(2, _roSystem.stSystemData.sAddress);
                row->get(3, _roSystem.stSystemData.sPort);
                row->get(4, _roSystem.stSystemData.sAuthInfo);
                row->get(5, _roSystem.stSystemData.sCreatedAt);
                row->get(6, _roSystem.stSystemData.sUpdatedAt);
                return 0;
            }
            return 1;
        }

        Response processMgmtGetServicesId(int _Id)
        {
            if(_Id < 1)
                return ErrorResp{"Id must be greater than 0.", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/services/{id}"}.getResp();

            ServiceDefinition oServiceDefinition;
            uint8_t status = processServiceDefinition(_Id, oServiceDefinition);
            if(status)
                return ErrorResp{"Service definition with id of '" + std::to_string(_Id) + "' does not exist", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/services/{id}"}.getResp();

            return Response{ oServiceDefinition.createServiceDefinition() };
        }

        Response processMgmtGetServices(int page, int item_per_page, std::string sort_field, std::string direction)
        {
            ServiceDefinitionList oServiceDefinitionList;
            oServiceDefinitionList.uCount = 0;

            std::string sQuery = "SELECT * FROM service_definition";

            if(sort_field.size() != 0)
            {
                if( sort_field.compare("id") == 0 ||  sort_field.compare("createdAt") == 0 || sort_field.compare("updatedAt") == 0 )
                {
                    sQuery += ( sort_field.compare("id") == 0 ? std::string(" ORDER BY id") :
                              ( sort_field.compare("createdAt") == 0 ? std::string(" ORDER BY created_at") :
                              ( sort_field.compare("updatedAt") == 0 ? std::string(" ORDER BY updated_at") : "")));

                    if(direction.size() != 0)
                    {
                        if( direction.compare("ASC") != 0 || direction.compare("DESC") != 0)
                        {
                            sQuery += " " + direction;
                        }
                        else
                        {
                            return ErrorResp{"Unknown parameter '" + direction + "'", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/services"}.getResp();
                        }
                    }
                }
                else
                {
                    return ErrorResp{"Sortable field with reference '" + sort_field + "' is not available", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/services"}.getResp();
                }
            }

            if(item_per_page > 0)
                sQuery += " LIMIT " + std::to_string(item_per_page);

            if(page > 0)
                sQuery += " OFFSET " + std::to_string(page);

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    ServiceDefinition oServiceDefinition;
                    row->get(0, oServiceDefinition.stServDefData.sId);
                    row->get(1, oServiceDefinition.stServDefData.sServiceDefinition);
                    row->get(2, oServiceDefinition.stServDefData.sCreatedAt);
                    row->get(3, oServiceDefinition.stServDefData.sUpdatedAt);

                    oServiceDefinitionList.vServDef.push_back(oServiceDefinition.stServDefData);
                    oServiceDefinitionList.uCount++;
                } while( row->next() );
            }

            return Response{ oServiceDefinitionList.createServiceDefinitionList() };
        }

        uint8_t processServiceDefinition(int _Id, ServiceDefinition &_roServiceDefinition)
        {
            std::string sQuery = "SELECT * FROM service_definition WHERE id = '" + std::to_string(_Id) + "';";

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                row->get(0, _roServiceDefinition.stServDefData.sId);
                row->get(1, _roServiceDefinition.stServDefData.sServiceDefinition);
                row->get(2, _roServiceDefinition.stServDefData.sCreatedAt);
                row->get(3, _roServiceDefinition.stServDefData.sUpdatedAt);
                return 0;
            }
            return 1;
        }

        Response processMgmtGetServiceDef(std::string serviceDef, int page, int item_per_page, std::string sort_field, std::string direction)
        {
            std::string trimmedServDef = serviceDef;
            toLowerAndTrim(trimmedServDef);
            const char *p = trimmedServDef.c_str();
            int i = 0;
            for(; ;++i)
            {
                if(p[i] == '\0') break;
                if(p[i] == '?') break;
            }

            std::string sResp = "{\"data\": [";
            int uCount = 0;

            std::string sQuery = "SELECT id FROM service_definition WHERE service_definition = '" + (i == trimmedServDef.size() ? trimmedServDef : trimmedServDef.substr(0,i)) + "'";
            std::string sID;
            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    row->get(0, sID);
                } while( row->next() );
            }
            else
            {
                return ErrorResp{"Empty response from service_definition table", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
            }

            sQuery = "SELECT id FROM service_registry WHERE service_id = '" + sID + "'";

            if(sort_field.size() != 0)
            {
                if( sort_field.compare("id") == 0 ||  sort_field.compare("createdAt") == 0 || sort_field.compare("updatedAt") == 0 )
                {
                    sQuery += ( sort_field.compare("id") == 0 ? std::string(" ORDER BY id") :
                              ( sort_field.compare("createdAt") == 0 ? std::string(" ORDER BY created_at") :
                              ( sort_field.compare("updatedAt") == 0 ? std::string(" ORDER BY updated_at") : "")));

                    if(direction.size() != 0)
                    {
                        if( direction.compare("ASC") != 0 || direction.compare("DESC") != 0)
                        {
                            sQuery += " " + direction;
                        }
                        else
                        {
                            return ErrorResp{"Unknown parameter '" + direction + "'", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
                        }
                    }
                }
                else
                {
                    return ErrorResp{"Sortable field with reference '" + sort_field + "' is not available", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
                }
            }

            if(item_per_page > 0)
                sQuery += " LIMIT " + std::to_string(item_per_page);

            if(page > 0)
                sQuery += " OFFSET " + std::to_string(page);

            if (auto row = db.fetch(sQuery.c_str()) )
            {
                do{
                    row->get(0, sID);

                    ServiceRegistryEntry oServiceRegistryEntry;
                    uint8_t status = processServiceRegistryEntry(std::stoi(sID), oServiceRegistryEntry);

                    switch(status)
                    {
                        case 1: return ErrorResp{"Empty response from service_registry table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
                        case 2: return ErrorResp{"Empty response from service_definition table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
                        case 3: return ErrorResp{"Empty response from system_ table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
                        case 4: return ErrorResp{"Empty response from service_registry_interface_connection table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
                        case 5: return ErrorResp{"Empty response from service_interface table", 400, "BAD_PAYLOAD", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
                    }

                    sResp += oServiceRegistryEntry.createRegistryEntry() + std::string(",");
                    uCount++;
                } while( row->next() );
            }
            else
            {
                return ErrorResp{"Empty response from service_registry table", 400, "INVALID_PARAMETER", "serviceregistry/mgmt/servicedef/{serviceDefinition}"}.getResp();
            }

            sResp.back() = ']';
            sResp += ",\"count\":" + std::to_string(uCount) + "}";

            return Response {sResp};
        }
};

#endif   /* _ENDPOINTS_MGMTGET_H_ */
