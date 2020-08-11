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
};


#endif //CORE_CPP_GENERALUTILS_H
