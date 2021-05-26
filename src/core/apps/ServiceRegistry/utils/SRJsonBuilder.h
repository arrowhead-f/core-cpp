#ifndef _UTILS_SRJSONBUILDER_H_
#define _UTILS_SRJSONBUILDER_H_

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

    void addMetaData(std::string key, const std::string &value)
    {
        addObj(key, dbMetadataToJsonMetadata(value));
    }

    std::string dbMetadataToJsonMetadata(std::string _dbMeta)
    {
        std::string sJsonMeta = "{";
        std::string token;
        std::istringstream tokenStream(_dbMeta);
        while(std::getline(tokenStream, token, ','))
        {
            std::istringstream tokenStream2(token);
            std::string key;
            std::string value;
            std::getline(tokenStream2, key, '=');
            if ( key.size() != 0)
            {
                std::getline(tokenStream2, value, '=');
                if ( value.size() != 0)
                    sJsonMeta += "\"" + key + "\":"+"\""+value+"\",";
            }
        }

        if(sJsonMeta.size() != 1)
            sJsonMeta.back() = '}';
        else
            sJsonMeta += "}";

        return sJsonMeta;
    }

};

#endif /* _UTILS_SRJSONBUILDER_H_ */
