#ifndef _ARROWHEAD_LOGGER_SYSLOG_H_
#define _ARROWHEAD_LOGGER_SYSLOG_H_

#ifndef _ARROWHEAD_LOGGER_H_
  #error "Do not include this file directly."
#endif

#include <syslog.h>

namespace logger {

    namespace detail {

        namespace {

            template<char ...before>struct OBefore {

                template<char ...str>struct OCurrent {
                    template<typename ...Args>
                    static constexpr auto convert() {
                        static_assert(sizeof...(Args) == 0, "Too many arguments.");
                        return compile_time::string<before...>::chars;
                    }
                };

                template<char c1>struct OCurrent<c1> {
                    template<typename ...Args>
                    static constexpr auto convert() {
                        static_assert(sizeof...(Args) == 0, "Too many arguments.");
                        return compile_time::string<before..., c1>::chars;
                    }
                };

                template<char c1, char c2, char ...str>struct OCurrent<c1, c2, str...> {
                    template<typename ...Args>
                    static constexpr auto convert() {
                        return OBefore<before..., c1>::template OCurrent<c2, str...>::template convert<Args...>();
                    }
                };

                template<char c2, char ...str>struct OCurrent<'}', c2, str...> {
                    template<typename ...Args>
                    static constexpr auto convert() {
                        static_assert(sizeof...(Args) == 6666, "Wrong format.");
                    }
                };

                template<char ...str>struct OCurrent<'{' , '}', str...> {
                    template<typename T, typename ...Args>
                    static constexpr auto convert()
                    -> typename std::enable_if<std::is_same<typename std::decay<T>::type, int>::value, decltype(OBefore<before..., '%', 'd'>::template OCurrent<str...>::template convert<Args...>())>::type {
                        return OBefore<before..., '%', 'd'>::template OCurrent<str...>::template convert<Args...>();
                    }
                    template<typename T, typename ...Args>
                    static constexpr auto convert()
                    -> typename std::enable_if<std::is_same<typename std::decay<T>::type, unsigned>::value, decltype(OBefore<before..., '%', 'u'>::template OCurrent<str...>::template convert<Args...>())>::type {
                        return OBefore<before..., '%', 'u'>::template OCurrent<str...>::template convert<Args...>();
                    }
                    template<typename T, typename ...Args>
                    static constexpr auto convert()
                    -> typename std::enable_if<std::is_same<typename std::decay<T>::type, double>::value, decltype(OBefore<before..., '%', 'f'>::template OCurrent<str...>::template convert<Args...>())>::type {
                        return OBefore<before..., '%', 'f'>::template OCurrent<str...>::template convert<Args...>();
                    }
                    template<typename T, typename ...Args>
                    static constexpr auto convert()
                    -> typename std::enable_if<std::is_same<typename std::decay<T>::type, const char*>::value, decltype(OBefore<before..., '%', 's'>::template OCurrent<str...>::template convert<Args...>())>::type {
                        return OBefore<before..., '%', 's'>::template OCurrent<str...>::template convert<Args...>();
                    }
                    template<typename T, typename ...Args>
                    static constexpr auto convert()
                    -> typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value, decltype(OBefore<before..., '%', 's'>::template OCurrent<str...>::template convert<Args...>())>::type {
                        return OBefore<before..., '%', 's'>::template OCurrent<str...>::template convert<Args...>();
                    }
                };

                template<char ...str>struct OCurrent<'{' , '{', str...> {
                    template<typename ...Args>
                    static constexpr auto convert() {
                        return OBefore<before..., '{'>::template OCurrent<str...>::template convert<Args...>();
                    }

                };

                template<char ...str>struct OCurrent<'}' , '}', str...> {
                    template<typename ...Args>
                    static constexpr auto convert() {
                        return OBefore<before..., '}'>::template OCurrent<str...>::template convert<Args...>();
                    }

                };

            };
        }

        template<typename T>auto to_printable(T &&t) -> typename std::enable_if<std::is_fundamental<std::decay_t<T>>::value, T>::type {
            return t;
        }

        template<typename T>auto to_printable(T &&t) -> typename std::enable_if<std::is_same<const char *const&, T>::value, T>::type {
            return t;
        }

        template<typename T>auto to_printable(T &&t) -> typename std::enable_if<std::is_same<std::decay_t<T>, std::string>::value, const char*>::type {
            return t.c_str();
        }

        template<unsigned S, char ...str, typename ...Args>void print_log(Args&& ...args) {
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wformat-security"
            syslog(S, OBefore<>::template OCurrent<' ', '<', 'd', 'e', 'b', 'u', 'g', '>', ' ', ' ', str...>::template convert<Args...>(), to_printable(std::forward<Args>(args))...);
            #pragma GCC diagnostic pop
        }

        template<char ...str, typename ...Args>void print_debug(const char *sfile, int sline, Args&& ...args) {
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wformat-security"
            syslog(LOG_DEBUG, OBefore<>::template OCurrent<' ', '<', 'd', 'e', 'b', 'u', 'g', '>', ' ', str...>::template convert<const char*, int, Args...>(), sfile, sline, to_printable(std::forward<Args>(args))...);
            #pragma GCC diagnostic pop
        }

    }  // namespace detail

    inline bool init(int loglevel, const char *name, const char*) {
        setlogmask(LOG_UPTO(loglevel));
        openlog(name, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
        return true;
    }

}  // namespace logger

#endif  /* _ARROWHEAD_LOGGER_SYSLOG_H_ */
