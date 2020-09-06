#ifndef _ARROWHEAD_LOGGER_SYSLOG_H_
#define _ARROWHEAD_LOGGER_SYSLOG_H_

#ifndef _ARROWHEAD_LOGGER_H_
  #error "Do not include this file directly."
#endif

/* decide which one to use */
#ifdef USE_SYSLOG
  #include <syslog.h>
#else
  #include <cstdio>

  #define LOG_ERR        1
  #define LOG_WARNING    2
  #define LOG_INFO       4
  #define LOG_DEBUG      5
#endif

namespace logger {

    namespace detail {

        template<typename>struct format {
            static constexpr char fmt[] = "%s";
        };

        template<>struct format<char> {
            static constexpr char fmt[] = "%c";
        };

        template<>struct format<int> {
            static constexpr char fmt[] = "%d";
        };

        template<>struct format<long> {
            static constexpr char fmt[] = "%dl";
        };

        template<>struct format<long long> {
            static constexpr char fmt[] = "%dll";
        };

        template<>struct format<unsigned int> {
            static constexpr char fmt[] = "%u";
        };

        template<>struct format<unsigned long> {
            static constexpr char fmt[] = "%ul";
        };

        template<>struct format<unsigned long long> {
            static constexpr char fmt[] = "%dll";
        };

        template<>struct format<double> {
            static constexpr char fmt[] = "%f";
        };

        template<typename T>constexpr inline auto fix(const T &t) {
            return t;
        }

        template<>inline auto fix<std::string>(const std::string& t) {
            return t.c_str();
        }

        static constexpr const char error[] = "<error>  ";
        static constexpr const char warn[]  = "<warning>  ";
        static constexpr const char info[]  = "<info>  ";
        static constexpr const char debug[] = "<debug>  ";

        #ifdef USE_SYSLOG
            //                 //syslog(static_cast<int>(S), std::forward<Args>(args)...);
            template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S == LOG_ERR, void>::type {
                syslog(S, logger::detail::concat<error, logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            }

            template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S == LOG_WARNING, void>::type {
                syslog(S, logger::detail::concat<warn, logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            }

            template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S == LOG_INFO, void>::type {
                syslog(S, logger::detail::concat<info, logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            }

            template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S == LOG_DEBUG, void>::type {
                syslog(S, logger::detail::concat<debug, logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            }
        #else
            //template<unsigned S, typename ...Args>void print_log(const char *fmt, Args&& ...args) {
            //    // add log level to the format
            //    //std::fprintf(stderr, logger::detail::concat<"<info>", logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            //    std::fprintf(stderr, fmt, std::forward<Args>(args)...);
            //}
            template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S == LOG_ERR, void>::type {
                std::fprintf(stderr, logger::detail::concat<error, logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            }

            template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S == LOG_WARNING, void>::type {
                std::fprintf(stderr, logger::detail::concat<warn, logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            }

            template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S == LOG_INFO, void>::type {
                std::fprintf(stderr, logger::detail::concat<info, logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            }

            template<unsigned S, typename ...Args>auto print_log(Args&& ...args) -> typename std::enable_if<S == LOG_DEBUG, void>::type {
                std::fprintf(stderr, logger::detail::concat<debug, logger::detail::format<Args>::fmt...>(), logger::detail::fix<Args>(args)...);
            }
        #endif

    }  // namespace detail


    #ifdef USE_SYSLOG
      inline bool init(int loglevel, const char *name, const char*) {
          setlogmask(LOG_UPTO(loglevel));
          openlog(name, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
          return true;
      }
    #else
      inline bool init(int loglevel, const char*, const char*) {
          return true;
      }
    #endif

}  // namespace logger

#endif  /* _ARROWHEAD_LOGGER_SYSLOG_H_ */
