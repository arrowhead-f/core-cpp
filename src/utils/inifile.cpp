#include "inifile.h"

#include <iostream>

#include <cstring>
#include <cctype>
#include <string>
#include <map>


bool INIFile::parse(const char *section /* = nullptr */, bool strict /* = false */) {

    // check the file
    if (!in.is_open())
        throw std::runtime_error{ "Cannot read the INI file." };

    in.clear();  // since C++11 this should be done by seekg
    in.seekg(0); // rewind
    if (!in)
        throw std::runtime_error{ "Cannot rewind the INI file." };

    bool collect = !section || !strict; // whether we want to collect the config settings
    std::string gr;                     // the group processed currently

    std::string line;
    while(std::getline(in, line)) {
        // comments and empty lines
        if (line[0] == ';' || line.empty() || (line[0] == '#' && line[1] == ' '))
            continue;

        // it is a secton tag
        if (line[0] == '[') {
            gr.clear();
            bool check = false;
            for(std::string::size_type i = 1; i < line.length(); i++) {
                if (check) {
                    if (line[i] == '#' || line[i] != ';')
                        break;

                    if (!std::isblank(line[i]))
                        return false;
                }

                if (line[i] == ']') {
                    check = true;
                    continue;
                }

                gr.push_back(line[i]);
            }
            if (!check)
                return false;

            collect = (section == nullptr || gr == section);
            continue;
        }

        if (collect) {
            // read key-values
            auto &val = values[gr];
            if (!val.empty())
                val.append(" ");
            val.append(line);
        }
    }

    return true;
}


std::map<std::string, std::string> INIFile::parse_opt(const char *opts) {

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
          options.insert_or_assign(pname, pval);
        #else
          options[pname] = pval;
        #endif
    }

    // Done with the modifiable input string
    delete[] buf;
    return options;
}
