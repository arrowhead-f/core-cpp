#ifndef _CORE_CAUTILITIES_H_
#define _CORE_CAUTILITIES_H_


#include <ctime>
#include <string>


namespace CAUtilities {

    inline std::string getTime() {
        std::time_t rawtime;
        std::tm *timeinfo;
        char buffer[20]; buffer[19] = 0;

        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);

        std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
        return buffer;
    }

    inline std::string getCertStatus(const std::string &valid_after, const std::string &valid_before, bool revoked) {

        if(revoked)
            return "revoked";

        const std::string current_time = getTime();
        if(valid_before < current_time || valid_after > current_time)
            return "expired";

        return "good";
    }

}

#endif  /* _CORE_CAUTILITIES_H_ */
