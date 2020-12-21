#ifndef _ARROWHEAD_AUTHORIZER_H_
#define _ARROWHEAD_AUTHORIZER_H_

#include "core/Core.h"

#include <iostream>

template<typename DBPool, typename RB>class Authorizer final : public Core<DBPool, RB> {

public:
    using Core<DBPool, RB>::Core;

    int handleGET(const std::string &uri, std::string &response) final {
        std::cout << "handle get";

        response = "handle get";
        return 0;
    }

    int handlePOST(const std::string &uri, std::string &response, const std::string &payload) final {
        response = "handle post";
        return 0;
    }

    int handlePUT(const std::string &uri, std::string &response, const std::string &payload) final {
        response = "handle put";
        return 0;
    }

    int handlePATCH(const std::string &uri, std::string &response, const std::string &payload) final {
        response = "handle patch";
        return 0;
    }

    int handleDELETE(const std::string &uri, std::string &response, const std::string &payload) final {
        response = "handle delete";
        return 0;
    }

};

#endif  /*_ARROWHEAD_AUTHORIZER_H_*/
