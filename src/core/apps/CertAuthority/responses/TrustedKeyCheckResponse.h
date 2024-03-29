#ifndef _CORE_CA_TRUSTEDCHECKRESPONSE_H_
#define _CORE_CA_TRUSTEDCHECKRESPONSE_H_

#include <string>

#include "utils/json.h"


class TrustedKeyCheckResponse : public JsonBuilder {

    public:

        using JsonBuilder::JsonBuilder;

        struct ID {
            static constexpr const char *key = "id";
            long value;
        };

        struct CreatedAt {
            static constexpr const char *key = "createdAt";
            const std::string &value;
        };


        struct Description {
            static constexpr const char *key = "description";
            const std::string &value;
        };

};

#endif  /* _CORE_CA_TRUSTEDCHECKRESPONSE_H_ */
