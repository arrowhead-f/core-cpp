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
#ifndef CORE_AUTHORIZATION_QUERIES_INTERCLOUDCHECK_H_
#define CORE_AUTHORIZATION_QUERIES_INTERCLOUDCHECK_H_

#include <iostream>
#include <string>
#ifdef __cpp_lib_format
  #include <format>
#endif

#include "utils/logger.h"

#include "core/helpers/CoreQueryStr.h"
#include "../requests/models/InterCloudCheck.h"

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
/// cloud_gatekeeper_relay
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | cloud_id                     | bigint(20)    | NO   | MUL | NULL                |                               |
/// | relay_id                     | bigint(20)    | NO   | MUL | NULL                |                               |
/// | created_at                   | timestamp     | NO   |     | current_timestamp() |                               |
/// | updated_at                   | timestamp     | NO   |     | current_timestamp() | on update current_timestamp() |
///
/// cloud_gateway_relay
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | cloud_id                     | bigint(20)    | NO   | MUL | NULL                |                               |
/// | relay_id                     | bigint(20)    | NO   | MUL | NULL                |                               |
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


    template<typename DB>class InterCloudCheck {

        private:

            DB &db;  ///< The underlying database connection.

        public:

            InterCloudCheck(DB &db) : db{ db } {}

            /// Returns all records for the given id.
            auto checkRule(const Models::InterCloudCheck &model) const {

                std::string q = R"sql(SELECT t2.consumer_cloud_id, c1.name, c1.operator, c1.secure, c1.neighbor, c1.own_cloud, c1.authentication_info, c1.created_at, c1.updated_at,
                                             p1.id AS provider, t1.interface_id AS interface

                                      FROM authorization_inter_cloud_interface_connection t1
                                      INNER JOIN authorization_inter_cloud t2 ON (t1.authorization_inter_cloud_id = t2.id)
                                      INNER JOIN cloud   c1 ON (c1.id = t2.consumer_cloud_id)
                                      INNER JOIN system_ p1 ON (p1.id = t2.provider_system_id)

                                      INNER JOIN cloud_gatekeeper_relay gk ON (gk.cloud_id = t2.consumer_cloud_id)
                                      INNER JOIN cloud_gateway_relay    gw ON (gw.cloud_id = t2.consumer_cloud_id)

                                      WHERE)sql";

                q.append(" t2.service_id = ").append(std::to_string(model.serviceDefinitionId));

                q.append(" AND c1.neighbor = ").append(model.neighbor ? "1" : "0");
                q.append(" AND c1.secure = ").append(model.secure ? "1" : "0");
                q.append(" AND c1.operator = ").append(db.escape(model.op));
                q.append(" AND c1.name = ").append(db.escape(model.name));
                q.append(" AND c1.authentication_info = ").append(db.escape(model.authenticationInfo));

                q.append(" AND (gk.relay_id IN (");
                for(const auto &x : model.gatekeeperRelayIds) {
                    q.append(std::to_string(x)).append(",");
                }
                q[q.length()-1] = ')';

                q.append(") AND (gw.relay_id IN (");
                for(const auto &x : model.gatewayRelayIds) {
                    q.append(std::to_string(x)).append(",");
                }
                q[q.length()-1] = ')';

                q.append(") AND ( ");

                bool first = true;
                for(const auto &x : model.providerIdsWithInterfaceIds) {
                    if (!first)
                        q.append(" OR ");
                    else
                        first = false;

                    q.append(" (p1.id = ").append(std::to_string(x.first)).append(" AND t1.interface_id IN (");
                    for(auto &y : x.second) {
                        q.append(std::to_string(y)).append(",");
                    }
                    q[q.length()-1] = ')';
                    q.append(")");
                }
                q.append(" ) ORDER BY p1.id ASC, t1.id ASC");

                (debug{ } << fmt("Queries::InterCloudCheck: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

    };  // class InterCloudCheck


    #ifdef __cpp_deduction_guides
      template<typename DB>
      InterCloudCheck(DB&) -> InterCloudCheck<DB>;
    #endif

    template<typename DB>auto GetInterCloudCheck(DB &db) {
        return InterCloudCheck{ db };
    }


}  // namespace Queries

#endif /* CORE_AUTHORIZATION_QUERIES_INTERCLOUDCHECK_H_ */
