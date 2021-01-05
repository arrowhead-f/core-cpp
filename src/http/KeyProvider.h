#ifndef _HTTP_KEYPROVIDER_H_
#define _HTTP_KEYPROVIDER_H_


#include <stdexcept>
#include <string>
#include <fstream>


class KeyProvider {

    private:

        bool isValid = false;

    public:

        const std::string sslCert;
        const std::string sslCertType;
        const std::string sslKey;
        const std::string sslKeyType;
        const std::string keyPasswd;
        const std::string caInfo;

        const std::string sslCertData;
        const std::string sslKeyData;
        const std::string caInfoData;

    public:

        KeyProvider() = default;

        KeyProvider(const std::string &sslCert, const std::string &sslCertType, const std::string &sslKey, const std::string &sslKeyType, const std::string &keyPasswd, const std::string &caInfo)
            : isValid{ true }
            , sslCert{ sslCert }
            , sslCertType{ sslCertType }
            , sslKey{ sslKey }
            , sslKeyType{ sslKeyType }
            , keyPasswd{ keyPasswd }
            , caInfo{ caInfo }
            , sslCertData{ get_file_contents(sslCert) }
            , sslKeyData{ get_file_contents(sslKey) }
            , caInfoData{ get_file_contents(caInfo) }
        {}

        operator bool() const {
            return isValid;
        }

    private:

        static std::string get_file_contents(const std::string &filename) {
            if(std::ifstream in{ filename, std::ios::in | std::ios::binary }) {
                std::string contents;
                in.seekg(0, std::ios::end);
                contents.resize(in.tellg());
                in.seekg(0, std::ios::beg);
                in.read(&contents[0], contents.size());
                in.close();
                return contents;
            }
            throw std::runtime_error{ "Cannot initialize KeyProvider." };
        }

};


#endif   /* _HTTP_KEYPROVIDER_H_ */
