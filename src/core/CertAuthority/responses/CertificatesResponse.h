#ifndef _CORE_CA_CERTIFICATESRESPONSE_H_
#define _CORE_CA_CERTIFICATESRESPONSE_H_


#include <string>

#include "utils/json.h"


class CertificatesResponse : public JsonBuilder {

    public:

        using JsonBuilder::JsonBuilder;

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


        struct CreatedBy {
            static constexpr const char *key = "createdBy";
            const std::string &value;
        };

        struct ValidFrom {
            static constexpr const char *key = "validFrom";
            const std::string &value;
        };

        struct ValidUntil {
            static constexpr const char *key = "validUntil";
            const std::string &value;
        };

        struct RevokedAt {
            static constexpr const char *key = "revokedAt";
            const std::string &value;
            const bool is_null;
        };

        struct CommonName {
            static constexpr const char *key = "commonName";
            const std::string &value;
        };

        struct SerialNumber {
            static constexpr const char *key = "serialNumber";
            const std::string &value;
        };

        struct Status {
            static constexpr const char *key = "status";
            const std::string &value;
        };


};

#endif  /* _CORE_CA_CERTIFICATESRESPONSE_H_ */
