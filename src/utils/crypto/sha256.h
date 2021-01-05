#ifndef _ARROWHEAD_SHA256_H_
#define _ARROWHEAD_SHA256_H_


#include <openssl/sha.h>


namespace crypto {

    inline bool sha256(const std::string &input, std::string &md) {
        static constexpr const char* digits = "0123456789abcdef";

        SHA256_CTX context;
        if(!SHA256_Init(&context))
            return false;

        if(!SHA256_Update(&context, input.c_str(), input.length()))
            return false;

        unsigned char hash[SHA256_DIGEST_LENGTH];
        if(!SHA256_Final(hash, &context))
            return false;

        // convert the hash to std::string
        md.reserve(64);
        md.resize(64);
        for(auto i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            md[2 * i]     = digits[(hash[i] >> 4) & 0x0F];
            md[2 * i + 1] = digits[hash[i] & 0x0F];
        }

        return true;
    }

}

#endif  /* _ARROWHEAD_SHA256_H_ */
