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

        template<unsigned S, char ...str, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S==LOG_ERR, void>::type {
            #ifdef USE_SPDLOG
              daily_logger->error(compile_time::string<str...>::chars, std::forward<Args>(args)...);
            #else
              spdlog::error(compile_time::string<str...>::chars, std::forward<Args>(args)...);
            #endif
        }

        template<unsigned S, char ...str, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S==LOG_WARNING, void>::type {
            #ifdef USE_SPDLOG
              daily_logger->warn(compile_time::string<str...>::chars, std::forward<Args>(args)...);
            #else
              spdlog::warn(compile_time::string<str...>::chars, std::forward<Args>(args)...);
            #endif
        }

        template<unsigned S, char ...str, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S==LOG_INFO, void>::type {
            #ifdef USE_SPDLOG
              daily_logger->info(compile_time::string<str...>::chars, std::forward<Args>(args)...);
            #else
              spdlog::info(compile_time::string<str...>::chars, std::forward<Args>(args)...);
            #endif
        }

        template<unsigned S, char ...str, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S==LOG_DEBUG, void>::type {
            #ifdef USE_SPDLOG
              daily_logger->debug(compile_time::string<str...>::chars, std::forward<Args>(args)...);
            #else
              spdlog::debug(compile_time::string<str...>::chars, std::forward<Args>(args)...);
            #endif
        }

        template<char ...str, typename ...Args>void print_debug(char *sfile, int sline, Args&& ...args) {
            #ifdef USE_SPDLOG
              daily_logger->debug(compile_time::string<str...>::chars, sfile, sline, std::forward<Args>(args)...);
            #else
              spdlog::debug(compile_time::string<str...>::chars, sfile, sline, std::forward<Args>(args)...);
            #endif
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
