/********************************************************************************
 * Copyright (c) 2022 BME
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   * Budapest University of Technology and Economics - implementation
 *     * ng201
 ********************************************************************************/
#ifndef CORE_AUTHORIZATION_QUERIES_INTERCLOUD_H_
#define CORE_AUTHORIZATION_QUERIES_INTERCLOUD_H_


#include <string>
#ifdef __cpp_lib_format
  #include <format>
#endif

#include "utils/logger.h"

#include "core/helpers/CoreQueryStr.h"
#include "../requests/models/InterCloud.h"

///
/// Tables used:
///
/// authorization_inter_cloud
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | created_at                   | timestamp     | NO   |     | current_timestamp() |                               |
/// | updated_at                   | timestamp     | NO   |     | current_timestamp() | on update current_timestamp() |
/// | consumer_cloud_id            | bigint(20)    | NO   | MUL | NULL                |                               |
/// | provider_system_id           | bigint(20)    | NO   | MUL | NULL                |                               |
/// | service_id                   | bigint(20)    | NO   | MUL | NULL
///
/// authorization_inter_cloud_interface_connection
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | authorization_inter_cloud_id | bigint(20)    | NO   | MUL | NULL                |                               |
/// | interface_id                 | bigint(20)    | NO   | MUL | NULL                |                               |
/// | created_at                   | timestamp     | NO   |     | current_timestamp() |                               |
/// | updated_at                   | timestamp     | NO   |     | current_timestamp() | on update current_timestamp() |
///
/// cloud
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | operator                     | varchar(255)  | NO   | MUL | NULL                |                               |
/// | name                         | varchar(255)  | NO   |     | NULL                |                               |
/// | secure                       | int(1)        | NO   |     | 0                   |                               |
/// | neighbor                     | int(1)        | NO   |     | 0                   |                               |
/// | own_cloud                    | int(1)        | NO   |     | 0                   |                               |
/// | authentication_info          | varchar(2047) | YES  |     | NULL                |                               |
/// | created_at                   | timestamp     | NO   |     | current_timestamp() |                               |
/// | updated_at                   | timestamp     | NO   |     | current_timestamp() | on update current_timestamp() |
///
/// system_
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | system_name                  | varchar(255)  | NO   | MUL | NULL                |                               |
/// | address                      | varchar(255)  | NO   |     | NULL                |                               |
/// | port                         | int(11)       | NO   |     | NULL                |                               |
/// | authentication_info          | varchar(2047) | YES  |     | NULL                |                               |
/// | created_at                   | timestamp     | NO   |     | current_timestamp() |                               |
/// | updated_at                   | timestamp     | NO   |     | current_timestamp() | on update current_timestamp() |
///
/// service_definition
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | service_definition           | varchar(255)  | NO   | UNI | NULL                |                               |
/// | created_at                   | timestamp     | NO   |     | current_timestamp() |                               |
/// | updated_at                   | timestamp     | NO   |     | current_timestamp() | on update current_timestamp() |
///
/// service_interface
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | interface_name               | varchar(255)  | YES  | UNI | NULL                |                               |
/// | created_at                   | timestamp     | NO   |     | current_timestamp() |                               |
/// | updated_at                   | timestamp     | NO   |     | current_timestamp() | on update current_timestamp() |
///


namespace Queries {


    template<typename DB>class InterCloud {

        private:

            DB &db;  ///< The underlying database connection.

        public:

            InterCloud(DB &db) : db{ db } {}

            /// Returns all records for the given id.
            auto getRecords(unsigned long id) const {
                const std::string q = R"sql(SELECT t2.id, t2.created_at, t2.updated_at,
                                                   t2.consumer_cloud_id, c1.name, c1.operator, c1.secure, c1.neighbor, c1.own_cloud, c1.authentication_info, c1.created_at, c1.updated_at,
                                                   t2.provider_system_id, p1.system_name, p1.address, p1.port, p1.authentication_info, p1.created_at, p1.updated_at,
                                                   t2.service_id, s1.service_definition, s1.created_at, s1.updated_at,
                                                   t1.interface_id, s2.interface_name, s2.created_at, s2.updated_at

                                            FROM authorization_inter_cloud_interface_connection t1
                                            INNER JOIN authorization_inter_cloud t2 ON (t1.authorization_inter_cloud_id = t2.id)

                                            INNER JOIN cloud   c1 ON (c1.id = t2.consumer_cloud_id)
                                            INNER JOIN system_ p1 ON (p1.id = t2.provider_system_id)
                                            INNER JOIN service_definition s1 ON (s1.id = t2.service_id)
                                            INNER JOIN service_interface s2 ON (s2.id = t1.interface_id)

                                            WHERE t2.id = )sql" + std::to_string(id);

                (debug{ } << fmt("Queries::InterCloud: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

            /// Returns the number of the records
            auto getRecords() const {
                const char *q = R"sql(SELECT COUNT(*)
                                      FROM authorization_inter_cloud t2
                                      INNER JOIN cloud   c1 ON (c1.id = t2.consumer_cloud_id)
                                      INNER JOIN system_ p1 ON (p1.id = t2.provider_system_id)
                                      INNER JOIN service_definition s1 ON (s1.id = t2.service_id))sql";

                (debug{ } << fmt("Queries::InterCloud: {}") << q).log(SOURCE_LOCATION);

                long count;
                db.fetch(q, count);
                return count;
            }

            /// Returns all records for the given id.
            auto getRecords(const std::vector<long> &ids) const {
                std::string q = R"sql(SELECT t2.id, t2.created_at, t2.updated_at,
                                             t2.consumer_cloud_id, c1.name, c1.operator, c1.secure, c1.neighbor, c1.own_cloud, c1.authentication_info, c1.created_at, c1.updated_at,
                                             t2.provider_system_id, p1.system_name, p1.address, p1.port, p1.authentication_info, p1.created_at, p1.updated_at,
                                             t2.service_id, s1.service_definition, s1.created_at, s1.updated_at,
                                             t1.interface_id, s2.interface_name, s2.created_at, s2.updated_at

                                      FROM authorization_inter_cloud_interface_connection t1
                                      INNER JOIN authorization_inter_cloud t2 ON (t1.authorization_inter_cloud_id = t2.id)

                                      INNER JOIN cloud   c1 ON (c1.id = t2.consumer_cloud_id)
                                      INNER JOIN system_ p1 ON (p1.id = t2.provider_system_id)
                                      INNER JOIN service_definition s1 ON (s1.id = t2.service_id)
                                      INNER JOIN service_interface s2 ON (s2.id = t1.interface_id)

                                      WHERE t2.id IN ()sql";
                for(auto id : ids) {
                    q.append(std::to_string(id));
                    q.append(",");
                }
                q[q.length()-1] = ')';
                q.append(" ORDER BY t2.id");

                (debug{ } << fmt("Queries::InterCloud: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

            /// Returns all records as specified by the query string..
            auto getRecords(const CoreQueryStr &qs) const {
                std::string q = R"sql(SELECT t2.id, t2.created_at, t2.updated_at,
                                             t2.consumer_cloud_id, c1.name, c1.operator, c1.secure, c1.neighbor, c1.own_cloud, c1.authentication_info, c1.created_at, c1.updated_at,
                                             t2.provider_system_id, p1.system_name, p1.address, p1.port, p1.authentication_info, p1.created_at, p1.updated_at,
                                             t2.service_id, s1.service_definition, s1.created_at, s1.updated_at,
                                             t1.interface_id, s2.interface_name, s2.created_at, s2.updated_at

                                      FROM authorization_inter_cloud_interface_connection t1
                                      INNER JOIN authorization_inter_cloud t2 ON (t1.authorization_inter_cloud_id = t2.id)

                                      INNER JOIN cloud   c1 ON (c1.id = t2.consumer_cloud_id)
                                      INNER JOIN system_ p1 ON (p1.id = t2.provider_system_id)
                                      INNER JOIN service_definition s1 ON (s1.id = t2.service_id)
                                      INNER JOIN service_interface s2 ON (s2.id = t1.interface_id)

                                      WHERE t2.id IN (
                                          SELECT tt2.id
                                          FROM authorization_inter_cloud tt2)sql";

                // add the ordering
                if (!qs.sortField().compare("createdAt")) {
                    q += R"sql( ORDER BY tt2.created_at )sql" + qs.sortOrder() + R"sql(, tt2.id ASC)sql";
                }
                if (!qs.sortField().compare("updatedAt")) {
                    q += R"sql( ORDER BY tt2.updated_at )sql" + qs.sortOrder() + R"sql(, tt2.id ASC)sql";
                }
                else /* [[likely]] */ {
                    if (!qs.sortOrder().compare("ASC"))
                        q += R"sql( ORDER BY tt2.id ASC)sql";
                    else
                        q += R"sql( ORDER BY tt2.id DESC)sql";
                }

                // add the pages
                if (qs.hasPage()) {
                    const auto offset = qs.getPage() * qs.getItemPerPage(); // calculate the offset
                    q.append(" LIMIT " + std::to_string(qs.getItemPerPage()) + " OFFSET " + std::to_string(offset));
                }

                q.append(")");
                if (!qs.sortField().compare("id")) {
                    if (!qs.sortOrder().compare("ASC"))
                        q += R"sql( ORDER BY t2.id ASC)sql";
                    else
                        q += R"sql( ORDER BY t2.id DESC)sql";
                }

                (debug{ } << fmt("Queries::InterCloud: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

            /// Deletes the intercloud record by id.
            bool delRecord(unsigned long id) const {
                const std::string q = "DELETE FROM authorization_inter_cloud WHERE id = " + std::to_string(id);

                (debug{ } << fmt("Queries::InterCloud: {}") << q).log(SOURCE_LOCATION);

                return static_cast<bool>(db.query_and_check(q.c_str()));
            }

            /// Inserts records based on the data stored in the model.
            auto insRecord(const Models::InterCloud &model) const {

                std::vector<long> ids;

                db::TransactionLock transaction{ db };

                for(auto provider : model.providerIds) {
                    for(auto service : model.serviceDefIds) {

                        #ifdef __cpp_lib_format
                          std::string q = std::format("INSERT INTO authorization_inter_cloud (consumer_cloud_id, provider_system_id, service_id) VALUES ({}, {}, {})", model.cloudId, provider, service);
                        #else
                          std::string q = "INSERT INTO authorization_inter_cloud (consumer_cloud_id, provider_system_id, service_id) VALUES (";
                          q.append(std::to_string(model.cloudId)).append(",").append(std::to_string(provider)).append(",").append(std::to_string(service)).append(")");
                        #endif

                        (debug{ } << fmt("Queries::InterCloud: {}") << q).log(SOURCE_LOCATION);

                        long id;
                        if (db.insert(q.c_str(), id)) {

                            for(auto iface : model.interfaceIds) {

                                #ifdef __cpp_lib_format
                                  std::string iq = std::format("INSERT INTO authorization_inter_cloud_interface_connection (authorization_inter_cloud_id, interface_id) VALUES ({}, {})", id, iface);
                                #else
                                  std::string iq = "INSERT INTO authorization_inter_cloud_interface_connection (authorization_inter_cloud_id, interface_id) VALUES (";
                                  iq.append(std::to_string(id)).append(",").append(std::to_string(iface)).append(")");
                                #endif

                                (debug{ } << fmt("Queries::InterCloud: {}") << iq).log(SOURCE_LOCATION);

                                long iid;
                                if (!db.insert(iq.c_str(), iid)) {
                                    throw std::runtime_error{ "Cannot insert interface." };
                                }
                            }

                            ids.push_back(id);
                        }
                        else {
                            throw std::runtime_error{ "Cannot insert record." };
                        }
                    }
                }

                transaction.commit();

                return ids;
            }

    };  // class InterCloud


    #ifdef __cpp_deduction_guides
      template<typename DB>
      InterCloud(DB&) -> InterCloud<DB>;
    #endif

    template<typename DB>auto GetInterCloud(DB &db) {
        return InterCloud{ db };
    }


}  // namespace Queries

#endif /* CORE_AUTHORIZATION_QUERIES_INTERCLOUD_H_ */
