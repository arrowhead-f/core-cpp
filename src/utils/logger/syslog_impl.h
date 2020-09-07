#ifndef _ARROWHEAD_LOGGER_SYSLOG_H_
#define _ARROWHEAD_LOGGER_SYSLOG_H_

#ifndef _ARROWHEAD_LOGGER_H_
  #error "Do not include this file directly."
#endif

#include <syslog.h>
#include "spdlog/fmt/fmt.h"

namespace logger {

    namespace detail {

        template<unsigned S, typename ...Args>void print_log(const char *fmt, Args&& ...args) {
            // copy the string and replace {}...
            syslog(S, "%s", fmt::format(fmt, std::forward<Args>(args)...).c_str());
        }

    }  // namespace detail

    inline bool init(int loglevel, const char *name, const char*) {
        setlogmask(LOG_UPTO(loglevel));
        openlog(name, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
        return true;
    }

}  // namespace logger

#endif  /* _ARROWHEAD_LOGGER_SYSLOG_H_ */
