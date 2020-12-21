#ifndef _ARROWHEAD_LOGGER_H_
#define _ARROWHEAD_LOGGER_H_

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef TESTRUNNER
  #undef USE_SYSLOG
  #undef USE_NILLOG
  #undef USE_SPDLOG

  #define USE_NILLOG
#endif

#if (defined USE_SYSLOG && defined USE_NILLOG) || (defined USE_SYSLOG && defined USE_SPDLOG) || (defined USE_SPDLOG && defined USE_NILLOG)
#error "Multiple logger defined. Define only one."
#endif

/* common declarations */
//#include <string>

#define SOURCE_LOCATION __FILE__, __LINE__

#include <utility>
#include <tuple>
#include <iostream>
#include <sstream>

/* either for std::apply, std::invoke or std::ref */
#include <functional>

namespace logger {

#ifndef __cpp_lib_invoke
    namespace detail {
        /// See Making Pointers to Members Callable, http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0312r1.html
        template<typename F, typename... Args> auto invoke(F f, Args&&... args) -> decltype(std::ref(f)(std::forward<Args>(args)...)) {
            return std::ref(f)(std::forward<Args>(args)...);
        }
    }  // namespace detail
#endif

#ifndef __cpp_lib_apply
    namespace detail {
        template <class F, class Tuple, std::size_t... I> constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
#ifndef  __cpp_lib_invoke
                return detail::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
#else
                // This implementation is valid since C++20 (via P1065R2)
                // In C++17, a constexpr counterpart of std::invoke is actually needed here
                return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
#endif
        }

        template <class F, class Tuple> constexpr decltype(auto) apply(F&& f, Tuple&& t) {
            return detail::apply_impl(
                std::forward<F>(f), std::forward<Tuple>(t),
                std::make_index_sequence<std::tuple_size<typename std::remove_reference<Tuple>::type>::value>{});
        }
    }  // namespace detail
#endif

    namespace detail {
        template<unsigned count, template<unsigned...> class meta_functor, unsigned... indices>
        struct apply_range {
            typedef typename apply_range<count - 1, meta_functor, count - 1, indices...>::result result;
        };

        template<template<unsigned...> class meta_functor, unsigned... indices>
        struct apply_range<0, meta_functor, indices...> {
            typedef typename meta_functor<indices...>::result result;
        };
    }

    namespace compile_time {
        template<char... str> struct string {
            static inline constexpr const char chars[sizeof...(str) + 1] = {str..., '\0'};
        };

        template<char... str> constexpr  const char  string<str...>::chars[sizeof...(str)+1];

        template<typename lambda_str_type> struct string_builder {
            template<unsigned... indices> struct produce {
                typedef string<lambda_str_type{}.chars[indices]...> result;
            };
        };
    }

}  // namespace logger

#define fmt(string_literal)                                                                            \
    ([]{                                                                                               \
        struct constexpr_string_type { const char *chars = string_literal; };                          \
        return typename logger::detail::apply_range<sizeof(string_literal) - 1,                        \
            logger::compile_time::string_builder<constexpr_string_type>::template produce>::result{};  \
    }())

/* include specific header */
#if defined USE_SYSLOG
#include "logger/syslog_impl.h"
#endif

#if defined USE_SPDLOG || defined USE_SCRLOG
#include "logger/spdlog_impl.h"
#endif

namespace logger {

    template<unsigned S>
    struct nil_log_stream_op {
        constexpr nil_log_stream_op() = default;

        constexpr void log() const {}
        constexpr void log(const char*, int) const {}
    };

    template<unsigned S, typename T>
    inline auto operator<<(const nil_log_stream_op<S> &a, T&&) {
        return a;
    }

#ifdef USE_NILLOG
    inline bool init() {
        return true;
    }
#endif

    template<unsigned S>
    struct log_stream_op_start {};

    template<unsigned S, char ...str>
    struct log_stream_op {

        template<typename ...Args>
        struct stream {

            template<typename T> using next = log_stream_op<S, str...>::template stream<Args..., T>;
            using me = log_stream_op<S, str...>::template stream<Args...>;

            const std::tuple<Args...> args;

            constexpr stream() {}

            constexpr stream(std::tuple<Args...> &&args) : args{std::move(args)} { }

            void log() {
                #ifdef __cpp_lib_apply
                    std::apply([this](auto &&... args) {
                        logger::detail::print_log<S, str...>(std::forward<decltype(args)>(args)...);
                    }, args);
                #else
                    detail::apply([this](auto&&... args) {
                        logger::detail::print_log<S, str...>(std::forward<decltype(args)>(args)...);
                    }, args);
                #endif
            }

            template<unsigned V = S> auto log(const char *sfile, int sline) -> typename std::enable_if<V == LOG_DEBUG, void>::type {
                static_assert(V == S, "Do not change the severity level in method call.");

                #ifdef __cpp_lib_apply
                    std::apply([this, sfile, sline](auto&&... args) {
                        logger::detail::print_debug<' ', '<', '{', '}', ':', '{', '}', '>', ' ', ' ', str...>(sfile, sline, std::forward<decltype(args)>(args)...);
                    }, args);
                #else
                    detail::apply([this, sfile, sline](auto&&... args) {
                        logger::detail::print_debug<' ', '<', '{', '}', ':', '{', '}', '>', ' ', ' ', str...>(sfile, sline, std::forward<decltype(args)>(args)...);
                    }, args);
                #endif
            }

            /// source location is only available for debug messages
            template<unsigned V = S> auto log(const char*, int) -> typename std::enable_if<V != LOG_DEBUG, void>::type {
                static_assert(V == S, "Do not change the severity level in method call.");

                log();
            }
        };

    };

    template<unsigned S, char ...str> auto operator<<(log_stream_op_start<S> &&start, compile_time::string<str...> &&fmt) {
        return typename log_stream_op<S, str...>::template stream<>{};
    }

    template<typename X, typename T> auto operator<<(X &&lstr, const T &val) -> typename std::enable_if<std::is_same<X, typename X::me>::value, typename X::template next<T>>::type {
        return typename X::template next<T> {
                std::tuple_cat( std::move(lstr).args, std::tie(val) )
        };
    }

}  // namespace log


/////////////////////////////////////////////////////
/// loggers with different log levels are defined ///
/// outside the namespace, so ADL will kick in    ///
/////////////////////////////////////////////////////
#ifdef USE_NILLOG
using error   = logger::nil_log_stream_op<LOG_ERR>;
using warning = logger::nil_log_stream_op<LOG_WARNING>;
using info    = logger::nil_log_stream_op<LOG_INFO>;
using debug   = logger::nil_log_stream_op<LOG_DEBUG>;
#else
using error   = logger::log_stream_op_start<LOG_ERR>;
using warning = logger::log_stream_op_start<LOG_WARNING>;
using info    = logger::log_stream_op_start<LOG_INFO>;
#ifdef NDEBUG
using debug   = logger::nil_log_stream_op<LOG_DEBUG>;
#else
using debug   = logger::log_stream_op_start<LOG_DEBUG>;
#endif
#endif

#endif  /* _ARROWHEAD_LOGGER_H_ */
