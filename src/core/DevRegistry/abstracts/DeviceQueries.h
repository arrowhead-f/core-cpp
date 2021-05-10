#ifndef _CORE_DEVICEQUERIES_H_
#define _CORE_DEVICEQUERIES_H_


#include <iostream>
#include <string>

#include "Models.h"
#include "utils/logger.h"


template<typename DB>class DeviceQueries {

    private:

        DB &db;  ///< The underlying database connection.

    public:

        DeviceQueries(DB &db) : db{ db } {}


        /// Returns the number of devices.
        long getDeviceCount() const {
            const std::string q = "SELECT COUNT(*) FROM device";
            (debug{ } << fmt("DeviceQuery: {}") << q).log(SOURCE_LOCATION);

            long count = 0;
            db.fetch(q.c_str(), count);
            return count;
        }

        /// Returns all teh devices.
        auto getDevices(const std::string &page, const std::string &item_per_page, const std::string &sort_by, const std::string &sort_dir) const {
            std::string q = "SELECT id, created_at, updated_at, device_name, address, mac_address, authentication_info FROM device ORDER BY " + sort_by + " " + sort_dir;
            if (!page.empty()) {
                const auto p = std::stoul(page);
                const auto i = std::stoul(item_per_page);
                const auto o = p * i; // calculate the offset

                q.append(" LIMIT " + item_per_page + " OFFSET " + std::to_string(o));
            }

            (debug{ } << fmt("DeviceQuery: {}") << q).log(SOURCE_LOCATION);
            return db.fetch(q.c_str());
        }

        /// Returns the device given by its id.
        /// \param id               the id of the device
        auto getDevice(unsigned long id) const {
            const std::string q = "SELECT id, created_at, updated_at, device_name, address, mac_address, authentication_info FROM device WHERE id = " + std::to_string(id);

            (debug{ } << fmt("DeviceQuery: {}") << q).log(SOURCE_LOCATION);
            return db.fetch(q.c_str());
        }

        /// Deletes the device given by its id.
        /// \param id               the id of the device
        void delDevice(unsigned long id) const {
            const std::string q = "DELETE FROM device WHERE id = " + std::to_string(id);

            (debug{ } << fmt("DeviceQuery: {}") << q).log(SOURCE_LOCATION);
            db.query(q.c_str());
        }

        /// Updates the device. Not given values will be set to NULL.
        /// \param id               the id of the device
        /// \param dev              the description of the device
        bool putDevice(unsigned long id, const Models::Device &dev) const {
            const std::string q = "UPDATE device SET device_name = " + emptyToNull(dev.dev_name) + ", mac_address = " + emptyToNull(dev.mac_address) + ", address = " + emptyToNull(dev.address) + ", authentication_info = " + emptyToNull(dev.auth_info) + " WHERE id = " + std::to_string(id);

            (debug{ } << fmt("DeviceQuery: {}") << q).log(SOURCE_LOCATION);
            return db.query_and_check(q.c_str());
        }

        /// Updates the device. Not given values won't be changed.
        /// \param id               the id of the device
        /// \param device           the description of the device
        bool patchDevice(unsigned long id, const Models::Device &dev) const {
            std::string q = "UPDATE device SET";

            if (dev.dev_name)
                q += " device_name = " + db.escape(dev.dev_name) + ",";
            if (dev.mac_address)
                q += " mac_address = " + db.escape(dev.mac_address) + ",";
            if (dev.address)
                q += " address = " + db.escape(dev.address) + ",";
            if (dev.auth_info)
                q += " authentication_info = " + db.escape(dev.auth_info) + ",";

            q.pop_back();
            q += " WHERE id = " + std::to_string(id);

            (debug{ } << fmt("DeviceQuery: {}") << q).log(SOURCE_LOCATION);
            return db.query_and_check(q.c_str());
        }

        /// Creates a new device.
        /// \param id               the id of the device
        /// \param device           the description of the device
        bool postDevice(long &id, const Models::Device &dev) const {
            std::string q = "INSERT INTO device (device_name, mac_address, address, authentication_info) VALUES (" + emptyToNull(dev.dev_name) + ", " + emptyToNull(dev.mac_address) + ", " + emptyToNull(dev.address) + ", " + emptyToNull(dev.auth_info) + ")";
            (debug{ } << fmt("DeviceQuery: {}") << q).log(SOURCE_LOCATION);
            return db.insert(q.c_str(), id);
        }

    private:

        std::string emptyToNull(const char *data) const {
            if (!data)
                return "NULL";
            else
                return db.escape(data);
        }

};


template<typename DB>
DeviceQueries(DB&) -> DeviceQueries<DB>;


template<typename DB>auto DeviceQuery(DB &db) {
    return DeviceQueries{ db };
}

#endif /* _CORE_DEVICEQUERIES_H_ */
