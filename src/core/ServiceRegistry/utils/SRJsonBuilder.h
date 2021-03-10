
#pragma once

#include <string>
#include "utils/json.h"

class SRJsonBuilder : public JsonBuilder {

public:
    void addStr(std::string key, std::string value){
        write<std::string>(key.c_str(), value);
    }

    void addInt(std::string key, std::string value){
        write<int>(key.c_str(), std::stoi(value));
    }

    void addInt(std::string key, int value){
        write<int>(key.c_str(), value);
    }

    void addDbl(std::string key, std::string value){
        write<double>(key.c_str(), std::stod(value));
    }

    void addDbl(std::string key, double value){
        write<double>(key.c_str(), value);
    }

    void addBool(std::string key, std::string value){
        write<double>(key.c_str(), std::stod(value));
    }

    void addBool(std::string key, bool value){
        write<bool>(key.c_str(), value);
    }

    void addObj(std::string key, const std::string &value){
        writeObj(key.c_str(), value);
    }
};
