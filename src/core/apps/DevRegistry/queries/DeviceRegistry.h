/********************************************************************************
 * Copyright (c) 2021 BME
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
#ifndef _CORE_DEVREGISTRY_QUERIES_DEVICEREGISTRY_H_
#define _CORE_DEVREGISTRY_QUERIES_DEVICEREGISTRY_H_


#include "../models/Device.h"
#include "../models/DeviceRegistry.h"
#include "utils/logger.h"


namespace Queries {

    template<typename DB>class DeviceRegistries {

        private:

            DB &db;

        public:

            DeviceRegistries(DB &db) : db{ db } {}

            /// Get the device registry entry by the given id.
            /// \param req          the request
            /// \param id           the id of the device
            auto getById(unsigned long id) {
                const std::string q = "SELECT t2.id, t2.created_at, t2.updated_at, t2.version, t2.end_of_validity, t1.id, t1.created_at, t2.updated_at, t1.device_name, t1.mac_address, t1.address, t1.authentication_info FROM device t1 INNER JOIN device_registry t2 ON (t1.id = t2.device_id) WHERE t2.id = " + std::to_string(id);

                (debug{ } << fmt("Queries::DeviceRegistry: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

            /// Returns the total number of device registry entries.
            long getCount() const {
                const std::string q = "SELECT COUNT(*) FROM device_registry";
                (debug{ } << fmt("Queries::DeviceRegistry: {}") << q).log(SOURCE_LOCATION);

                long count = 0;
                db.fetch(q.c_str(), count);
                return count;
            }

            /// Returns all the device registry entries filtered by the given attributess.
            auto getAll(const std::string &page, const std::string &item_per_page, const std::string &sort_by, const std::string &sort_dir) const {
                std::string q = "SELECT t2.id, t2.created_at, t2.updated_at, t2.version, t2.end_of_validity, t1.id, t1.created_at, t2.updated_at, t1.device_name, t1.mac_address, t1.address, t1.authentication_info FROM device t1 INNER JOIN device_registry t2 ON (t1.id = t2.device_id) ORDER BY " + sort_by + " " + sort_dir;
                if (!page.empty()) {
                    const auto p = std::stoul(page);
                    const auto i = std::stoul(item_per_page);
                    const auto o = p * i; // calculate the offset

                    q.append(" LIMIT " + item_per_page + " OFFSET " + std::to_string(o));
                }

                (debug{ } << fmt("Queries::DeviceRegistry: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

            /// Deletes the device registry entry given by its id.
            /// \param id               the id of the device registry
            void remById(unsigned long id) const {
                const std::string q = "DELETE FROM device_registry WHERE id = " + std::to_string(id);

                (debug{ } << fmt("Queries::DeviceRegistry: {}") << q).log(SOURCE_LOCATION);
                db.query(q.c_str());
            }

            auto postDeviceRegistry(unsigned long id, const Models::DeviceRegistry &dr) {

                const auto devId = findOrCreateDevice(dr.device);
                const std::string q = "INSERT INTO device_registry *( VALUES ()";

                (debug{ } << fmt("Queries::DeviceRegistry: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

            void putDeviceRegistry(unsigned long id, const Models::DeviceRegistry &dr) {

            }

            auto patchDeviceRegistry(unsigned long id, const Models::DeviceRegistry &dr) {

                const auto devId = findOrCreateDevice(dr.device);
                const std::string q = "SELECT t2.id, t2.created_at, t2.updated_at, t2.version, t2.end_of_validity, t1.id, t1.created_at, t2.updated_at, t1.device_name, t1.mac_address, t1.address, t1.authentication_info FROM device t1 INNER JOIN device_registry t2 ON (t1.id = t2.device_id) WHERE t2.id = " + std::to_string(id);

                (debug{ } << fmt("Queries::DeviceRegistry: {}") << q).log(SOURCE_LOCATION);
                return db.fetch(q.c_str());
            }

            auto findOrCreateDevice(const Models::Device &dev) {
                const std::string q = "SELECT id FROM device WHERE device_name = " + db.escape(dev.dev_name) + (dev.mac_address ? " AND mac_address = " + db.escape(dev.mac_address) : "");

                long id = 0;
                if (!db.fetch(q.c_str(), id)) {
                    const std::string r = "INSERT INTO device (device_name, mac_address, address, authentication_info) VALUES (" + emptyToNull(dev.dev_name) + ", " + emptyToNull(dev.mac_address) + ", " + emptyToNull(dev.address) + ", " + emptyToNull(dev.auth_info) + ")";
                    (debug{ } << fmt("Queries::DeviceRegistry: {}") << r).log(SOURCE_LOCATION);
                    db.insert(q.c_str(), id);
                }
                return id;
            }

            bool isDeviceRegistered(unsigned long id) {
                const std::string q = "SELECT id FROM device_registry WHERE device_id = " + std::to_string(id);

                long did = 0;
                if (!db.fetch(q.c_str(), did)) {
                    return false;
                }

                return true;
            }


            std::string emptyToNull(const char *data) const {
                if (!data)
                    return "NULL";
                else
                    return db.escape(data);
            }

    };  // class DeviceRegistries


    template<typename DB>
    DeviceRegistries(DB&) -> DeviceRegistries<DB>;


    template<typename DB>auto DeviceRegistry(DB &db) {
        return DeviceRegistries{ db };
    }

}  // namespace Queries

#endif /* _CORE_DEVREGISTRY_QUERIES_DEVICEREGISTRY_H_ */
