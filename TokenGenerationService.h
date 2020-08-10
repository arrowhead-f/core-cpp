#ifndef CORE_CPP_TOKENGENERATIONSERVICE_H
#define CORE_CPP_TOKENGENERATIONSERVICE_H

///STD Libs
#include <iostream>
#include <vector>
#include <string>
#include <map>

class TokenGenerationService {

    private:
        //TODO: logger

        static const std::string DOT = ".";
        static const std::string  JWT_CONTENT_TYPE = "JWT";

        bool sslEnabled;    //TODO: @Value(CommonConstants.$SERVER_SSL_ENABLED_WD)

        //TODO: Figure out Map<String, Object> arrowheadContext (maybe std::any)

        //CommonDBService commonDBService
        //TODO: Implement CommonDBService

        //ServiceInterfaceNameVerifier interfaceNameVerifier
        //TODO: Implement ServiceInterfaceNameVerifier

        std::string ownCloudName;
        std::string ownCloudOperator;
};


#endif //CORE_CPP_TOKENGENERATIONSERVICE_H
