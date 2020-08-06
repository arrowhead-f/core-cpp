#ifndef CORE_CPP_TOKENGENERATIONPROVIDERDTO_H
#define CORE_CPP_TOKENGENERATIONPROVIDERDTO_H

/// STD libs
#include <iostream>
#include <string>
#include <cassert>
#include <vector>

/// Header includes
#include "SystemRequestDTO.h"

using lgint = long int;
using MyInt = int;


class TokenGenerationProviderDTO {

private:
    static const lgint serialVersionUID = -8058322682102502369L;

    SystemRequestDTO provider;                   ///< See SystemRequestDTO.h
    std::vector<std::string> serviceInterfaces;  ///< Vector of strings containing service interfaces
    MyInt tokenDuration = -1;                    ///< Token duration in seconds

public:
    /// Default constructor
    TokenGenerationProviderDTO() = default;
    ///Constructor with parameters
    TokenGenerationProviderDTO(const SystemRequestDTO& provider, const MyInt tokenDuration, const vector<std::string>& serviceInterfaces) {
        assert(serviceInterfaces.empty());          ///< Asserts if serviceInterface is empty
        this->provider = provider;
        this->tokenDuration = tokenDuration;
        this->serviceInterfaces = serviceInterfaces;
    }


    /*!
    * Getter function returning provider
    * @return provider
    */
    SystemRequestDTO getProvider() const { return provider; }
    /*!
    * Getter function returning tokenDuration
    * @return tokenDuration
    */
    MyInt getTokenDuration() const { return tokenDuration; }
    /*!
    * Getter function returning tokenDuration
    * @return tokenDuration
    */
    std::vector<std::string> getServiceInterfaces() const { return serviceInterfaces; }


    /*!
    * Setter function setting provider
    * @param[in] provider
    */
    void setProvider(const SystemRequestDTO& provider) { this->provider = provider; }
    /*!
    * Setter function setting tokenDuration
    * @param[in] tokenDuration
    */
    void setTokenDuration(const MyInt tokenDuration) { this->tokenDuration = tokenDuration; }
    /*!
    * Setter function setting serviceInterfaces
    * @param[in] serviceInterfaces
    */
    void setServiceInterfaces(const std::vector<std::string>& serviceInterfaces) { this->serviceInterfaces = serviceInterfaces; }
};


#endif //CORE_CPP_TOKENGENERATIONPROVIDERDTO_H
