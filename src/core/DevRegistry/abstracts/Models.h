#ifndef _DEVREGSTRY_MODELS_H_
#define _DEVREGSTRY_MODELS_H_

namespace Models {

    struct Device {

        const char *dev_name    = nullptr;
        const char *mac_address = nullptr;
        const char *address     = nullptr;
        const char *auth_info   = nullptr;

    };

    struct DeviceRegistry {

        Device device;

        const char *end_of_validity = nullptr;
        int version = 1;
        std::string metadata;

    };

}

#endif  /* _DEVREGSTRY_MODELS_H_ */