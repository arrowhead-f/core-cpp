#ifndef CORE_CPP_TOKENGENERATIONSERVICE_H
#define CORE_CPP_TOKENGENERATIONSERVICE_H

///Header includes
#include "jwt/jwt.hpp"
#include "dto/SystemRequestDTO.h"
#include "dto/TokenGenerationProviderDTO.h"
#include "dto/TokenGenerationRequestDTO.h"
#include "dto/CloudRequestDTO.h"
#include "Utils/base64.h"
#include "cpp-jwt/include/jwt/jwt.hpp"
#include "Utils/TimeHelper.h"
#include "Utils/GeneralUtils.h"


///STD Libs
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <algorithm>
#include <functional>
#include <random>
#include <bitset>
#include <array>
#include <ctime>

using namespace jwt::params;

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

    private:

        ///Randomly generates JwtId
        /*!
         * First randomly generates a short which is then casted to a 16 bit bitset which is then made into a string which gets base64 encoded
         * @return base64 encoded JwtId
         */
        std::string generateJwtId(){
            std::random_device rd;
            auto seed_data = std::array<int, std::mt19937::state_size> {};
            std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
            std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
            std::mt19937 generator(seq);
            std::uniform_int_distribution<short> dis(std::numeric_limits<short>::min(),
                                                     std::numeric_limits<short>::max());

            std::string n = (std::bitset<16>(dis(generator)).to_string());

            return Base64::Encode(n);
        };

        ///Generates a token payload
        jwt::jwt_payload generateTokenPayload(const std::string consumerInfo, std::string service, const std::string intf, const int duration) {

            //TODO: logger

            jwt::jwt_payload payload = new jwt::jwt_payload();
            payload.add_claim("jti", generateJwtId());                              ///sets the generated JWT ID
            payload.add_claim("iss", CommonConstants.CORE_SYSTEM_AUTHORIZATION);    ///sets the issuer (CORE_SYSTEM_AUTHORIZATION = "Authorization")
            payload.add_claim("iat", time(0));                                ///sets issued at claim to current time
            payload.add_claim("nbf", TimeHelper::offsetInThePast(1));               ///sets not before to one minute in the past
            if (duration != 0)
                payload.add_claim("exp", TimeHelper::offsetInTheFuture((float)duration/CommonConstatns.CONVERSION_SECOND_TO_MINUTE);    ///If duration is not 0 sets the expiration time based on it
            payload.add_claim(CommonConstants.JWT_CLAIM_CONSUMER_ID, consumerInfo); ///sets the consumer id ("cid")
            payload.add_claim(CommonConstants.JWT_CLAIM_SERVICE_ID, GeneralUtils::toLower(service));    ///sets the service id ("sid")
            payload.add_claim(CommonConstants.JWT_CLAIM_INTERFACE_ID, intf);        ///sets the interface id ("iid")

            return payload;
        }



};


#endif //CORE_CPP_TOKENGENERATIONSERVICE_H
