#ifndef CORE_CPP_SYSTEMREQUESTDTO_H
#define CORE_CPP_SYSTEMREQUESTDTO_H

/// STD libs
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

/// Header includes
#include "hashUtils.h"


using lgint = long int;
using MyInt = int;

/*!
 * \struct
 * Used to ease the generation of hash from address, port and systemName
 */
struct hashHelper {
    std::string address;
    MyInt port;
    std::string systemName;
};


///DTO class to store and transfer system requests
class SystemRequestDTO {


private:
    static const lgint serialVersionUID = 3919207845374510215L;     ///< long int storing the serial version uid

    std::string systemName;                ///< string storing the name of the system
    std::string address;                   ///< string storing the address
    MyInt port;                            ///< integer storing the port
    std::string authenticationInfo;        ///< string storing authentication info


public:
    /// Default constructor
    SystemRequestDTO() = default;
    /// Constructor with parameters
    SystemRequestDTO(const std::string systemName, const std::string address, const MyInt port, const std::string authenticationInfo) : systemName{ systemName }, address{ address }, port{ port }, authenticationInfo{ authenticationInfo }{}


    /*!
     * Getter function returning systemName
     * @return systemName
     */
    std::string getSystemName() const { return systemName; }
    /*!
     * Getter function returning address
     * @return address
     */
    std::string getAddress() const { return address; }
    /*!
     * Getter function returning port
     * @return port
     */
    MyInt  getPort() const { return port; }
    /*!
     * Getter function returning authenticationInfo
     * @return authenticationInfo
     */
    std::string getAuthenticationInfo() const { return authenticationInfo; }


    /*!
     * Setter function setting systemName
     * @param[in] systemName
     */
    void setSystemName(const std::string systemName) { this->systemName = systemName; }
    /*!
     * Setter function setting address
     * @param[in] address
     */
    void setAddress(const std::string address) { this->address = address; }
    /*!
     * Setter function setting port
     * @param[in] port
     */
    void setPort(const MyInt port) { this->port = port; }
    /*!
     * Setter function setting authenticationInfo
     * @param[in] authenticationInfo
     */
    void setAuthenticationInfo(const std::string authenticationInfo) { this->authenticationInfo = authenticationInfo; }

    /*!
     * Generates hash code from address, port and systemName
     * @return hash generated from address, port and systemName
     */
    std::size_t hashCode() {
        hashHelper hashTemp{ address, port, systemName };
        std::size_t hash = 0;
        hash_combine(hash, hashTemp.address, hashTemp.port, hashTemp.systemName);
        return hash;
    }

    /*!
     * Overload of == operator
     * @param obj
     * @return Returns a boolean value showing if the two objects are equal
     */
    bool operator== (const SystemRequestDTO& obj) {
        ///If the object is compared with itself
        if (this == &obj)
            return true;

        return this->address == obj.getAddress() && this->systemName == obj.getSystemName() && this->port == obj.getPort();
    }

    /*!
     * Returns a string containing the data of the object
     * @return buffer
     */
    std::string toString() {
        std::ostringstream  buffer;
        buffer << "SystemRequestDTO[systemName='" << systemName << "', " << "address='" << address << "', " << "port='" << port << "', " << "authenticationInfo='" << authenticationInfo << "']";
        return buffer.str();
    }
};


#endif //CORE_CPP_SYSTEMREQUESTDTO_H
