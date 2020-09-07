#ifndef CORE_CPP_GENERALUTILS_H
#define CORE_CPP_GENERALUTILS_H

///STD Libs
#include <iostream>
#include <string>
#include <locale>
#include <algorithm>


class GeneralUtils {

public:

    ///Transforms a string to lower case
    std::string toLower(std::string data)
    {
        std::transform(data.begin(), data.end(), data.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return data;
    }

    std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
    {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
    {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
    {
        return ltrim(rtrim(str, chars), chars);
    }
};


#endif //CORE_CPP_GENERALUTILS_H
