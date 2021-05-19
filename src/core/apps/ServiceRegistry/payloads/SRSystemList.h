#ifndef _PAYLOADS_SRSYSTEMLIST_H_
#define _PAYLOADS_SRSYSTEMLIST_H_

#include "SRSystem.h"

class SRSystemList
{
    private:

    SRJsonBuilder         jResponse;
    std::vector<SRSystem> vSRSystem;
    uint uCount;

    public:

        std::string createSystemList()
        {
/*
            mainObject = json_object_new_object();
            json_object *jData = json_object_new_array();

            for(uint i = 0; i < vSystem.size(); ++i)
            {
                vSystem[i].fillJsonResponse();
                json_object_array_add(jData, vSystem[i].jResponse);
            }

            json_object_object_add(mainObject, "data", jData);

            addInt(mainObject, "count", uCount);

            return std::string(json_object_to_json_string(mainObject));
*/
         return std::string("");
        }

};

#endif /* _PAYLOADS_SRSYSTEMLIST_H_ */
