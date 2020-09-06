#ifndef _ARROWHEAD_LOGGER_SPDLOG_H_
#define _ARROWHEAD_LOGGER_SPDLOG_H_

#ifndef _ARROWHEAD_LOGGER_H_
  #error "Do not include this file directly."
#endif

#include <type_traits>

#include "spdlog/spdlog.h"
#ifdef USE_SPDLOG
  #include "spdlog/sinks/daily_file_sink.h"
  #include <memory>

  namespace logger {
      static std::shared_ptr<spdlog::logger> daily_logger { nullptr };
  }

#endif

#define LOG_ERR        1
#define LOG_WARNING    2
#define LOG_INFO       4
#define LOG_DEBUG      5

namespace logger {

    namespace detail {

        /// Helper class to create the format string compile time.
        template<typename>struct format {
            static constexpr char fmt[] = "{}";
        };

        /// This function should be use to convert the parameter of the logger function.
        /// And, well, for spdlog nothing should be done.
        template<typename T>constexpr inline auto fix(const T &t) {
            return t;
        }

        template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S==LOG_ERR, void>::type {
            #ifdef USE_SPDLOG
              daily_logger->error(logger::detail::concat<logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            #else
              spdlog::error(logger::detail::concat<logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            #endif
            //spdlog::error(std::forward<Args>(args)...);
        }

        template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S==LOG_WARNING, void>::type {
            #ifdef USE_SPDLOG
              daily_logger->warn(logger::detail::concat<logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            #else
              spdlog::warn(logger::detail::concat<logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            #endif
            //spdlog::warn(std::forward<Args>(args)...);
        }

        template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S==LOG_INFO, void>::type {
            #ifdef USE_SPDLOG
              daily_logger->error(logger::detail::concat<logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            #else
              spdlog::info(logger::detail::concat<logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            #endif
            //spdlog::info(std::forward<Args>(args)...);
        }

        template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S==LOG_DEBUG, void>::type {
            #ifdef USE_SPDLOG
              daily_logger->error(logger::detail::concat<logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            #else
              spdlog::debug(logger::detail::concat<logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            #endif
            //spdlog::debug(std::forward<Args>(args)...);
        }

    }  // namespace detail


    #ifdef USE_SPDLOG
      inline bool init(int loglevel, const char *name, const char *fname) {
          try {
              daily_logger = spdlog::daily_logger_mt(name, fname, 23, 59);
              switch(loglevel) {
                  case LOG_ERR:
                      daily_logger->set_level(spdlog::level::err);
                      break;
                  case LOG_WARNING:
                      daily_logger->set_level(spdlog::level::warn);
                      break;
                  case LOG_INFO:
                      daily_logger->set_level(spdlog::level::info);
                      break;
                  case LOG_DEBUG:
                      daily_logger->set_level(spdlog::level::debug);
                      break;
              }
          }
          catch(...) {
              return false;
          }
          return true;
      }
    #else
      inline bool init(int loglevel, const char*, const char*) {
          switch(loglevel) {
              case LOG_ERR:
                  spdlog::set_level(spdlog::level::err);
                  break;
              case LOG_WARNING:
                  spdlog::set_level(spdlog::level::warn);
                  break;
              case LOG_INFO:
                  spdlog::set_level(spdlog::level::info);
                  break;
              case LOG_DEBUG:
                  spdlog::set_level(spdlog::level::debug);
                  break;
          }
          return true;
      }
    #endif

}  // namespace logger

#endif  /* _ARROWHEAD_LOGGER_SYSLOG_H_ */
