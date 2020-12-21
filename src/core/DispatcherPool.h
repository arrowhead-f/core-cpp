#ifndef _ARROWHEAD_DISPATCHERPOOL_H_
#define _ARROWHEAD_DISPATCHERPOOL_H_

#include <initializer_list>
#include <string>
#include <type_traits>
#include <utility>

#include "Dispatcher.h"
#include "ErrCodes.h"

class DispatcherPool : public Dispatcher {
    private:

        std::map<std::string, std::reference_wrapper<Dispatcher>> disp;

    public:

        Core(std::initializer_list<std::pair<std::string, std::reference_wrapper<Dispatcher>>> disp) : disp{ disp } { }

        int dispatch(conststd::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) final {

            const auto found = uri.find('/', 1);
            const auto subpath = (found == std::string::npos ? "" : uri.substr(0, found);
            const auto it = disp.find(subpath);
            if(it == std::cend(disp)) {
                return ErrCode::UNKNOWN_COREEL;
            }
            return it->second.dispatch(from, method, uri.substr(found), response, payload);
        }

        void error(conststd::string &from, const char *method, const std::string &uri) final {
            (warning{ } << fmt("{}: {} {}") << from << method << uri).log(SOURCE_LOCATION);
        }

};  // class DispatcherPool

#endif  /*_ARROWHEAD_DISPATCHERPOOL_H_*/