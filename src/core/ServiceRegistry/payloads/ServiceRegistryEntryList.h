#pragma once

#include "ServiceRegistryEntry.h"

class ServiceRegistryEntryList
{
public:

    std::vector<ServiceRegistryEntry> vServiceRegistryEntry;
    uint uCount;

    std::string createRegistryEntryList()
    {
        SRJsonBuilder mainObject;
        std::vector<std::string> v;

        for(uint i = 0; i < vServiceRegistryEntry.size(); ++i)
            v.push_back(vServiceRegistryEntry[i].createRegistryEntry());

        mainObject.to_array<std::vector<std::string>::iterator>("data", v.begin(), v.end());
        mainObject.addInt("count", uCount);

        return mainObject.str();
    }
};
