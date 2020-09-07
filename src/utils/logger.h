#ifndef _ARROWHEAD_LOGGER_H_
#define _ARROWHEAD_LOGGER_H_

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#if (defined USE_SYSLOG && defined NILLOG) || (defined USE_SYSLOG && defined SPDLOG) || (defined USE_SPDLOG && defined NILLOG)
  #error "Multiple logger defined. Define only one."
#endif

/* common declarations */

#include <string>
#include <thread>

#define SOURCE_LOCATION __FILE__, __LINE__, std::to_string(std::this_thread::get_id())

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
        template<typename F, typename... Args>
        auto invoke(F f, Args&&... args) -> decltype(std::ref(f)(std::forward<Args>(args)...)) {
            return std::ref(f)(std::forward<Args>(args)...);
        }
    }  // namespace detail
    #endif

    #ifndef __cpp_lib_apply
    namespace detail {
        template <class F, class Tuple, std::size_t... I>
        constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
            #ifndef  __cpp_lib_invoke
              return detail::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
            #else
              // This implementation is valid since C++20 (via P1065R2)
              // In C++17, a constexpr counterpart of std::invoke is actually needed here
              return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
            #endif
        }

        template <class F, class Tuple>
        constexpr decltype(auto) apply(F&& f, Tuple&& t) {
            return detail::apply_impl(
                std::forward<F>(f), std::forward<Tuple>(t),
                std::make_index_sequence<std::tuple_size<typename std::remove_reference<Tuple>::type>::value>{});
        }
    }  // namespace detail
    #endif

    namespace detail {
        template<int...I> using is = std::integer_sequence<int, I...>;
        template<int N>   using make_is = std::make_integer_sequence<int, N>;

        constexpr auto size(const char* s) { int i = 0; while (*s != 0) { ++i; ++s; } return i; }

        template<const char*, typename, const char*, typename>
        struct concat_impl;

        template<const char* S1, int... I1, const char* S2, int... I2>
        struct concat_impl<S1, is<I1...>, S2, is<I2...>> {
            static constexpr const char value[] { S1[I1]..., S2[I2]..., 0 };
        };

        template<const char* S1>constexpr auto concat() {
            return S1;
        };

        template<const char* S1, const char* S2, const char* ...S>
        constexpr auto concat() {
            return concat<concat_impl<S1, make_is<size(S1)>, S2, make_is<size(S2)>>::value, S...>();
        };

    }  // namespace detail

}  // namespace logger


/* include specific header */
#if defined USE_SYSLOG
  #include "logger/syslog_impl.h"
#endif

#if defined USE_SPDLOG || defined USE_SCRLOG
  #include "logger/spdlog_impl.h"
#endif

namespace logger {

    template<unsigned S>struct nil_log_stream_op {
        constexpr nil_log_stream_op() = default;
        constexpr void log() const {}
    };

    template<unsigned S, typename T>inline auto operator<<(nil_log_stream_op<S> &a, const T&) {
        return a;
    }

    #ifdef USE_NILLOG
        inline bool init() {
            return true;
        }
    #endif

    template<unsigned, typename...>struct log_stream_op;

    template<unsigned S>struct log_stream_op<S> {
        const char *fmt;
        const std::tuple<> args;

        constexpr log_stream_op(const char *fmt) : fmt{ fmt } {}

        constexpr void log() const {
            logger::detail::print_log<S>(fmt);
        }

        constexpr void log(const char *file, const char *line, const std::string &thread) const {
            logger::detail::print_log<S>(fmt);
        }

    };

    template<unsigned S, typename... Args>struct log_stream_op {
        const char *fmt;
        const std::tuple<Args...> args;

        constexpr log_stream_op(const char *fmt, std::tuple<Args...>&& args) : fmt{ fmt }, args{ std::move(args) } {}

        constexpr void log() const {
            #ifdef __cpp_lib_apply
              std::apply([this](auto&&... args) { logger::detail::print_log<S>(fmt, std::forward<decltype(args)>(args)...); }, args);
            #else
              detail::apply([this](auto&&... args) { logger::detail::print_log<S>(fmt, std::forward<decltype(args)>(args)...); }, args);
            #endif
        }

        constexpr void log(const char *file, const char *line, const std::string &thread) const {
            #ifdef __cpp_lib_apply
              std::apply([this](auto&&... args) { logger::detail::print_log<S>(fmt, std::forward<decltype(args)>(args)...); }, args);
            #else
              detail::apply([this](auto&&... args) { logger::detail::print_log<S>(fmt, std::forward<decltype(args)>(args)...); }, args);
            #endif
        }

    };

    template<unsigned S, typename... A, typename B>inline auto operator<<(log_stream_op<S, A...> &&a, const B &b) -> typename std::enable_if<!std::is_same<typename std::decay<B>::type, const char*>::value && !std::is_same<typename std::decay<B>::type, char*>::value && !std::is_integral<B>::value && !std::is_floating_point<B>::value, log_stream_op<S, A..., std::string>>::type {
        std::ostringstream os;
        os << b;
        std::string s = os.str();
        return log_stream_op<S, A..., std::string>{ a.fmt, std::tuple_cat(a.args, std::tie(s)) };
    }

    template<unsigned S, typename... A>inline auto operator<<(log_stream_op<S, A...> &&a, const std::string &b) {
        return log_stream_op<S, A..., std::string>{ a.fmt, std::tuple_cat(a.args, std::tie(b)) };
    }

    template<unsigned S, typename... A, typename B>constexpr inline auto operator<<(log_stream_op<S, A...>&& a, const B b) -> typename std::enable_if<std::is_same<typename std::decay<B>::type, const char*>::value || std::is_same<typename std::decay<B>::type, char*>::value || std::is_integral<B>::value || std::is_floating_point<B>::value, log_stream_op<S, A..., B>>::type {
        return log_stream_op<S, A..., B>{ a.fmt, std::tuple_cat(a.args, std::tie(b)) };
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
  using error   = logger::log_stream_op<LOG_ERR>;
  using warning = logger::log_stream_op<LOG_WARNING>;
  using info    = logger::log_stream_op<LOG_INFO>;
  #ifdef NDEBUG
    using debug   = logger::nil_log_stream_op<LOG_DEBUG>;
  #else
    using debug   = logger::log_stream_op<LOG_DEBUG>;
  #endif
#endif

#endif  /* _ARROWHEAD_LOGGER_H_ */
