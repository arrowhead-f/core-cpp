#pragma once

#include "common.h"

using namespace std;

class ServiceQueryList
{
private:
    vector<servQueryData> vServQueryData;
    uint32_t unfilteredHits;

public:
    void fillContent();
    string createServiceQueryList();
};
