
#include "common.h"

class System
{
    private:
        json_object *mainObject;

    public:
        std::string sId;
        std::string sSystemName;
        std::string sAddress;
        std::string sPort;
        std::string sAuthInfo;
        std::string sCreatedAt;
        std::string sUpdatedAt;

        System(){}

        System(const char *_sJson, bool &_brSuccess)
        {
            mainObject = json_tokener_parse(_sJson);
            if(mainObject == NULL)
                _brSuccess = false;
            else
                _brSuccess = true;
        }

        bool validJSONPayload();
        std::string createSystemJSON();
};
