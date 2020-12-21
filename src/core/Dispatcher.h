#ifndef _ARROWHEAD_DISPATCHER_H_
#define _ARROWHEAD_DISPATCHER_H_

#include <string>
#include <utility>

class Dispatcher {

    public:

        virtual int dispatch(const std::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) = 0;
        virtual void error(const std::string &from, const char *method, const std::string &uri) = 0;

        static std::string subpath(const std::string &uri) {
            const auto found = uri.find('/', 1);
            return found == std::string::npos ? uri : uri.substr(found);
        }

};

#endif  /* _ARROWHEAD_DISPATCHER_H_ */
