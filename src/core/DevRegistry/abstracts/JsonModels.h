#ifndef _DEVREGSTRY_JSONMODELS_H_
#define _DEVREGSTRY_JSONMODELS_H_


#include "gason/gason.h"

#include "Models.h"


namespace JsonModels {

    /// Input JSON structure:
    /// {
    ///     "address": "string",
    ///     "authenticationInfo": "string",
    ///     "deviceName": "string",
    ///     "macAddress": "string"
    /// }
    class Device : public Models::Device {

        private:

            gason::JsonAllocator   allocator;
            gason::JsonValue       root;

            bool error = false;
            bool valid = false;

        public:

            static Device parse(std::string &content) {

                Device dev;
                auto &root = dev.root;

                gason::JsonParseStatus status = gason::jsonParse(content.data(), root, dev.allocator);

                if (status != gason::JSON_PARSE_OK) {
                    dev.error = true;
                    return dev;
                }

                if (auto node = root.child("address")) {
                    if (!node.isString())
                        return dev;
                    dev.address = node.toString();
                    dev.valid   = true;
                }
                if (auto node = root.child("deviceName")) {
                    if (!node.isString())
                        return dev;
                    dev.dev_name = node.toString();
                    dev.valid       = true;
                }
                if (auto node = root.child("macAddress")) {
                    if (!node.isString())
                        return dev;
                    dev.mac_address = node.toString();
                    dev.valid       = true;
                }
                if (auto node = root.child("authenticationInfo")) {
                    if (!node.isString())
                        return dev;
                    dev.auth_info = node.toString();
                    dev.valid    = true;
                }

                return dev;
            }

            auto empty() const {
                return !valid;
            }

            operator bool() const {
                return !error;
            }

            const char* validate() const {
                if (dev_name == nullptr)
                    return "Device name must have value.";
                if (mac_address == nullptr)
                    return "Device MAC address must have value.";
                if (address == nullptr)
                    return "Device address must have value.";

                return nullptr;
            }

    };


    /// Input JSON structure:
    /// {
    ///     "device": {
    ///         "address": "string",
    ///         "authenticationInfo": "string",
    ///         "deviceName": "string",
    ///         "macAddress": "string"
    ///     },
    ///     "endOfValidity": "string",
    ///     "metadata": {
    ///         "additionalProp1": "string",
    ///         "additionalProp2": "string",
    ///         "additionalProp3": "string"
    ///     },
    ///     "version": 0
    /// }
    class DeviceRegistry : public Models::DeviceRegistry {

        private:

            gason::JsonAllocator   allocator;
            gason::JsonValue       root;

            bool error  = false;  ///< Whether there was a parsing error.
            bool valid  = false;  ///< Whether the structure is valid.
            //bool device = false;  ///< Whether the device is given.


        public:

            static DeviceRegistry parse(std::string &content) {

                DeviceRegistry dev;
                auto &root = dev.root;

                gason::JsonParseStatus status = gason::jsonParse(content.data(), root, dev.allocator);

                if (status != gason::JSON_PARSE_OK) {
                    dev.error = true;
                    return dev;
                }

                if (auto node = root.child("device")) {
                    if (!node.isObject())
                        return dev;

                    //dev.device = true;
                    if (auto node = root.child("address")) {
                        if (!node.isString())
                            return dev;
                        dev.device.address = node.toString();
                        dev.valid   = true;
                    }
                    if (auto node = root.child("deviceName")) {
                        if (!node.isString())
                            return dev;
                        dev.device.dev_name = node.toString();
                        dev.valid       = true;
                    }
                    if (auto node = root.child("macAddress")) {
                        if (!node.isString())
                            return dev;
                        dev.device.mac_address = node.toString();
                        dev.valid       = true;
                    }
                    if (auto node = root.child("authenticationInfo")) {
                        if (!node.isString())
                            return dev;
                        dev.device.auth_info = node.toString();
                        dev.valid    = true;
                    }
                }


                if (auto node = root.child("endOfValidity")) {
                    if (!node.isString())
                        return dev;
                    dev.end_of_validity = node.toString();
                    dev.valid           = true;
                }
                if (auto node = root.child("version")) {
                    if (!node.isNumber())
                        return dev;
                    dev.version = node.toInt();
                    dev.valid   = true;
                }
                if (auto node = root.child("metadata")) {
                    if (!node.isString())
                        return dev;
                    dev.metadata = node.toString();
                    dev.valid    = true;
                }

                return dev;
            }

            auto empty() const {
                return !valid;
            }

            operator bool() const {
                return !error;
            }

            const char* validate() const {
                if (device.dev_name == nullptr)
                    return "Device name must have value.";
                if (device.mac_address == nullptr)
                    return "Device MAC address must have value.";
                if (device.address == nullptr)
                    return "Device address must have value.";

                return nullptr;
            }

    };

}

#endif  /* _DEVREGSTRY_JSONMODELS_H_ */