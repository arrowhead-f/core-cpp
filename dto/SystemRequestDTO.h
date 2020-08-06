#ifndef CORE_CPP_SYSTEMREQUESTDTO_H
#define CORE_CPP_SYSTEMREQUESTDTO_H

#include <string>
#include <iostream>


using lgint = long int;
using MyInt = int;

/*!
 * \struct
 * Used to ease the generation of hash from address, port and systemName
 */
struct hashHelper {
    string address;
    MyInt port;
    string systemName;
};


///DTO class to store and transfer system requests
class SystemRequestDTO {


private:
    static const lgint serialVersionUID = 3919207845374510215L;     ///< long int storing the serial version uid

    string systemName;                ///< string storing the name of the system
    string address;                   ///< string storing the address
    MyInt port;                       ///< integer storing the port
    string authenticationInfo;        ///< string storing authentication info


public:
    /// Default constructor
    SystemRequestDTO() = default;
    /// Constructor with parameters
    SystemRequestDTO(const string systemName, const string address, const MyInt port, const string authenticationInfo): systemName{systemName}, address{address}, port{port}, authenticationInfo{authenticationInfo}{}


    /*!
     * Getter function returning systemName
     * @return systemName
     */
    string getSystemName() const { return systemName; }
    /*!
     * Getter function returning address
     * @return address
     */
    string getAddress() const { return address; }
    /*!
     * Getter function returning port
     * @return port
     */
    MyInt  getPort() const { return port; }
    /*!
     * Getter function returning authenticationInfo
     * @return authenticationInfo
     */
    string getAuthenticationInfo() const { return authenticationInfo; }


    /*!
     * Setter function setting systemName
     * @param[in] systemName
     */
    void setSystemName(const string systemName) { this->systemName = systemName; }
    /*!
     * Setter function setting address
     * @param[in] address
     */
    void setAddress(const string address) { this->address = address; }
    /*!
     * Setter function setting port
     * @param[in] port
     */
    void setPort(const MyInt port) { this->port = port; }
    /*!
     * Setter function setting authenticationInfo
     * @param[in] authenticationInfo
     */
    void setAuthenticationInfo(const string authenticationInfo) { this->authenticationInfo = authenticationInfo; }

    /*!
     * Generates hash code from address, port and systemName
     * @return hash generated from address, port and systemName
     */
    size_t hashCode() {
        return std::hash<hashHelper> hash{address, port, systemName};
    }

    /*!
     * Overload of == operator
     * @param obj
     * @return Returns a boolean value showing if the two objects are equal
     */
    bool SystemRequestDTO::operator== (const SystemRequestDTO& obj) {
        ///If the object is compared with itself
        if (this == obj)
            return true;

        return this->address == obj.getAddress() && this->systemName == obj.getSystemName() && this->port == obj.getPort();
    }

    /*!
     * Returns a string containing the data of the object
     * @return buffer
     */
    string toString() {
        std::ostringstream  buffer;
        buffer << "SystemRequestDTO[systemName='" << systemName << "', " << "address='" << address << "', " << "port='" << port << "', " << "authenticationInfo='" << authenticationInfo << "']";
        return buffer.str();
    }
};


#endif //CORE_CPP_SYSTEMREQUESTDTO_H
