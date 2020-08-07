#ifndef CORE_CPP_CLOUDREQUESTDTO_H
#define CORE_CPP_CLOUDREQUESTDTO_H

/// STD libs
#include <iostream>
#include <string>
#include <cassert>
#include <vector>

/// Header includes
#include "hashUtils.h"

struct hashHelperCloud{
    std::string cloudOperator;
    std::string  name;
};

using llint = long long int;
using MyInt = int;

/// DTO class storing cloud requests
class CloudRequestDTO {

    private:
        static const long long int serialVersionUID = 7397917411719621910L;

        ///Private members
        std::string cloudOperator;
        std::string name;
        bool secure;
        bool neighbor;
        std::string authenticationInfo;
        std::vector<llint> gatekeeperRelayIds;
        std::vector<llint> gatewayRelayIds;

    public:
    ///Getter functions
        /*!
        * Getter function returning cloudOperator
        * @return cloudOperator
        */
        std::string getCloudOperator() const { return cloudOperator; }
        /*!
        * Getter function returning name
        * @return name
        */
        std::string getName() const { return name; }
        /*!
        * Getter function returning secure
        * @return secure
        */
        bool getSecure() const { return secure; }
        /*!
        * Getter function returning neighbor
        * @return neighbor
        */
        bool getNeighbor() const { return neighbor; }
        /*!
        * Getter function returning authenticationInfo
        * @return authenticationInfo
        */
        std::string getAuthenticationInfo() const { return authenticationInfo; }
        /*!
        * Getter function returning gatekeeperRelayIds
        * @return gatekeeperRelayIds
        */
        std::vector<llint> getGatekeeperRelayIds() const { return gatekeeperRelayIds; }
        /*!
        * Getter function returning gatewayRelayIds
        * @return gatewayRelayIds
        */
        std::vector<llint> getGatewayRelayIds() const { return gatewayRelayIds; }


    /// Setter functions
        /*!
        * Setter function setting cloudOperator
        * @param[in] cloudOperator
        */
        void setCloudOperator(const std::string cloudOperator) { this->cloudOperator = cloudOperator; }
        /*!
        * Setter function setting name
        * @param[in] name
        */
        void setName(const std::string name) { this->name = name; }
        /*!
        * Setter function setting secure
        * @param[in] secure
        */
        void setSecure(const bool secure) { this->secure = secure; }
        /*!
        * Setter function setting neighbor
        * @param[in] neighbor
        */
        void setNeighbor(const bool neighbor) { this->neighbor = neighbor; }
        /*!
        * Setter function setting authenticationInfo
        * @param[in] authenticationInfo
        */
        void setAuthenticationInfo(const std::string authenticationInfo) { this->authenticationInfo = authenticationInfo; }
        /*!
        * Setter function setting gatekeeperRelayIds
        * @param[in] gatekeeperRelayIds
        */
        void setGatekeeperRelayIds(const std::vector<llint>& gatekeeperRelayIds) { this->gatekeeperRelayIds = gatekeeperRelayIds; }
        /*!
        * Setter function setting gatewayRelayIds
        * @param[in] gatewayRelayIds
        */
        void setGatewayRelayIds(const std::vector<llint> gatewayRelayIds) { this->gatewayRelayIds = gatewayRelayIds; }


        /*!
        * Generates hash code from address, port and systemName
        * For hash_combine see hashUtils.h
        * @return hash generated from address, port and systemName
        */
        std::size_t hashCode() {
            hashHelperCloud hashTemp{ cloudOperator, name};
            std::size_t hash = 0;
            hash_combine(hash, hashTemp.cloudOperator, hashTemp.name);

            return hash;
    }
};


#endif //CORE_CPP_CLOUDREQUESTDTO_H
