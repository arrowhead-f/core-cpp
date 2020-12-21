#ifndef _HTTP_SERVER__HEADER_H_
#define _HTTP_SERVER__HEADER_H_

#include <string>

namespace http {
namespace server {

    struct Header {
        std::string name;
        std::string value;
    };

}  // namespace server
}  // namespace http

#endif  /* _HTTP_SERVER__HEADER_H_ */
