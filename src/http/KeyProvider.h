#ifndef _HTTP_KEYPROVIDER_H_
#define _HTTP_KEYPROVIDER_H_


#include <stdexcept>
#include <string>
#include <fstream>


class KeyProvider {

    private:

        bool isValid = false;

    public:

        struct KeyStore {
            std::string store;              ///< Where the certificates are stored.
            std::string alias;              ///< The filename.
            std::string password;           ///< The password for the key.

            std::string getCert() const {
                return store + "/" + alias + ".pem";
            }

            std::string getKey() const {
                return store + "/" + alias + ".key";
            }

        } keyStore;

        struct TrustStore {
            bool        mandatory = false;  ///< Whether client's authentication is mandatory or not.
            std::string store;              ///< The storage.
            std::string password;           ///< The password if used.

            std::string getCert() const {
                return store + "/ca.pem";
            }

        } trustStore;

    public:

        KeyProvider() = default;

        KeyProvider& loadKeyStore(const std::string &store, const std::string &alias, const std::string &password) {
            keyStore.store    = store;
            keyStore.alias    = alias;
            keyStore.password = password;

            isValid = true;

            return *this;
        }

        KeyProvider& loadTrustStore(const std::string &store, const std::string &password, bool mandatory = true) {
            trustStore.mandatory = mandatory;
            trustStore.store = store;
            trustStore.password = password;

            return *this;
        }

        operator bool() const {
            return isValid;
        }

};


#endif   /* _HTTP_KEYPROVIDER_H_ */
