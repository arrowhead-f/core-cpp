#include "parsers.h"

#include <cstring>
#include <cctype>
#include <string>
#include <map>

namespace parser {

std::map<std::string, std::string> parseOptions(const char *opts) {

    std::map<std::string, std::string> options;

    // Need a modifiable copy of the input string
    char *buf = new char[std::strlen(opts) + 1];
    if(!buf)
         return {};
    std::strcpy(buf, opts);

    char *cp = buf;

    while(*cp) {
        // Skip blanks before the parameter name
        if(std::isspace(*cp)) {
            cp++;
            continue;
        }

        // Get the parameter name
        char *pname = cp;
        while(*cp) {
            if(*cp == '=')
                break;
            if(std::isspace(*cp)) {
                *cp++ = '\0';
                while(*cp) {
                    if(!std::isspace(*cp))
                        break;
                    cp++;
                }
                break;
            }
            cp++;
        }

        // Check that there is a following '='
        if(*cp != '=') {
            delete[] buf;
            return {};
        }
        *cp++ = '\0';

        // Skip blanks after the '='
        while(*cp) {
            if(!std::isspace(*cp))
                break;
            cp++;
        }

        // Get the parameter value
        char *pval = cp;

        if (*cp != '\'') {
            char *cp2 = pval;
            while (*cp) {
                if(std::isspace(*cp)) {
                    *cp++ = '\0';
                    break;
                }
                if(*cp == '\\') {
                    cp++;
                    if(*cp != '\0')
                        *cp2++ = *cp++;
                }
                else
                    *cp2++ = *cp++;
            }
            *cp2 = '\0';
        }
        else {
            char *cp2 = pval;
            cp++;
            for(;;) {
                if(*cp == '\0') {
                    delete[] buf;
                    return {};
                }
                if(*cp == '\\') {
                    cp++;
                    if(*cp != '\0')
                        *cp2++ = *cp++;
                    continue;
                }
                if(*cp == '\'') {
                    *cp2 = '\0';
                    cp++;
                    break;
                }
                *cp2++ = *cp++;
            }
        }

        // Now that we have the name and the value, store the record.
        #ifdef __cpp_lib_map_try_emplace
          options.try_emplace(pname, pval);
        #else
          options.emplace(pname, pval);
        #endif
    }

    // Done with the modifiable input string
    delete[] buf;
    return options;
}

}
