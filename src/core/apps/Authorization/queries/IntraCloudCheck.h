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
#ifndef CORE_AUTHORIZATION_QUERIES_INTRACLOUDCHECK_H_
#define CORE_AUTHORIZATION_QUERIES_INTRACLOUDCHECK_H_

#include <iostream>
#include <string>
#ifdef __cpp_lib_format
  #include <format>
#endif

#include "utils/logger.h"

#include "core/helpers/CoreQueryStr.h"
#include "../requests/models/IntraCloudCheck.h"

///
/// Tables used:
///
/// authorization_intra_cloud
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | created_at                   | timestamp     | NO   |     | current_timestamp() |                               |
/// | updated_at                   | timestamp     | NO   |     | current_timestamp() | on update current_timestamp() |
/// | consumer_system_id           | bigint(20)    | NO   | MUL | NULL                |                               |
/// | provider_system_id           | bigint(20)    | NO   | MUL | NULL                |                               |
/// | service_id                   | bigint(20)    | NO   | MUL | NULL
///
/// authorization_intra_cloud_interface_connection
/// | id                           | bigint(20)    | NO   | PRI | NULL                | auto_increment                |
/// | authorization_intra_cloud_id | bigint(20)    | NO   | MUL | NULL                |                               |
/// | interface_id                 | bigint(20)    | NO   | MUL | NULL                |                               |
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


    template<typename DB>class IntraCloudCheck {

        private:

            DB &db;  ///< The underlying database connection.

        public:

            IntraCloudCheck(DB &db) : db{ db } {}

            /// Returns all records for the given id.
            auto checkRule(const Models::IntraCloudCheck &model) const {

                std::string q = R"sql(SELECT c1.id, c1.address, c1.port, c1.system_name, c1.authentication_info, c1.created_at, c1.updated_at,
                                             p1.id AS provider, t1.interface_id AS interface

                                      FROM authorization_intra_cloud_interface_connection t1
                                      INNER JOIN authorization_intra_cloud t2 ON (t1.authorization_intra_cloud_id = t2.id)
                                      INNER JOIN system_ c1 ON (c1.id = t2.consumer_system_id)
                                      INNER JOIN system_ p1 ON (p1.id = t2.provider_system_id)

                                      WHERE)sql";

                q.append(" t2.service_id = ").append(std::to_string(model.serviceDefinitionId));
                q.append(" AND c1.port = ").append(std::to_string(model.port));
                q.append(" AND c1.address = ").append(db.escape(model.address));
                q.append(" AND c1.system_name = ").append(db.escape(model.systemName));
                q.append(" AND c1.authentication_info = ").append(db.escape(model.authenticationInfo));

                q.append(" AND ( ");

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

                (debug{ } << fmt("Queries::IntraCloudCheck: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

    };  // class IntraCloudCheck


    #ifdef __cpp_deduction_guides
      template<typename DB>
      IntraCloudCheck(DB&) -> IntraCloudCheck<DB>;
    #endif

    template<typename DB>auto GetIntraCloudCheck(DB &db) {
        return IntraCloudCheck{ db };
    }


}  // namespace Queries

#endif /* CORE_AUTHORIZATION_QUERIES_INTRACLOUDCHECK_H_ */
