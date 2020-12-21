#ifndef _HTTP_SERVER_REPLYBUFFER_H_
#define _HTTP_SERVER_REPLYBUFFER_H_

#include <vector>

#include "asio/asio.hpp"
#include "Reply.h"

namespace http {
namespace server {

    /// Convert the reply into a vector of buffers. The buffers do not own the
    /// underlying memory blocks, therefore the reply object must remain valid and
    /// not be changed until the write operation has completed.
    std::vector<asio::const_buffer> to_buffers(http::server::Reply&);


} // namespace server
} // namespace http

#endif  /* _HTTP_SERVER_REPLYBUFFER_H_ */
