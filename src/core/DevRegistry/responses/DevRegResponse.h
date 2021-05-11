#ifndef _CORE_DR_DEVREGRESPONE_H_
#define _CORE_DR_DEVREGRESPONE_H_


#include <string>

#include "utils/json.h"


/// Helper class to craft device registry resposne.
class DevRegResponse : public JsonBuilder {

    public:

        using JsonBuilder::JsonBuilder;

        struct UnfilteredHits {
            static constexpr const char *key = "unfilteredHits";
            long value;
        };

        struct Count {
            static constexpr const char *key = "count";
            long value;
        };

        struct ID {
            static constexpr const char *key = "id";
            long value;
        };

        struct CreatedAt {
            static constexpr const char *key = "createdAt";
            const std::string &value;
        };

        struct UpdatedAt {
            static constexpr const char *key = "updatedAt";
            const std::string &value;
        };

        struct EndOfValidity {
            static constexpr const char *key = "endOfValidity";
            const std::string &value;
            const bool is_null;
        };

        struct Version {
            static constexpr const char *key = "version";
            int value;
        };

        struct Address {
            static constexpr const char *key = "address";
            const std::string &value;
        };

        struct AuthenticationInfo {
            static constexpr const char *key = "authenticationInfo";
            const std::string &value;
            const bool is_null;
        };

        struct DeviceName {
            static constexpr const char *key = "deviceName";
            const std::string &value;
        };

        struct MacAddress {
            static constexpr const char *key = "macAddress";
            const std::string &value;
        };
};

#endif  /* _CORE_DR_DEVREGRESPONE_H_ */
