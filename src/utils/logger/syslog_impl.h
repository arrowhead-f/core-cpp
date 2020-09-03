#ifndef _ARROWHEAD_LOGGER_SYSLOG_H_
#define _ARROWHEAD_LOGGER_SYSLOG_H_

#ifndef _ARROWHEAD_LOG_H_
  #error "Do not include this file directly."
#endif

/* decide which one to use */
#ifdef USE_SYSLOG
  #include <syslog.h>

  /* define a value for LOG_TRACE */
  #define _____MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
  #define LOG_TRACE      _____MAX(LOG_ERR, _____MAX(LOG_WARNING, _____MAX(LOG_NOTICE, _____MAX(LOG_INFO, LOG_DEBUG)))) + 1
#else
  #include <cstdio>

  #define LOG_ERR        1
  #define LOG_WARNING    2
  #define LOG_NOTICE     3
  #define LOG_INFO       4
  #define LOG_DEBUG      5
  #define LOG_TRACE      6
#endif

#include <utility>
#include <tuple>
#include <sstream>

/* either for std::apply, std::invoke or std::ref */
#include <functional>

namespace {

    #ifndef __cpp_lib_invoke
    namespace detail {
        /// See Making Pointers to Members Callable, http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0312r1.html
        template<typename F, typename... Args>
        auto invoke(F f, Args&&... args) -> decltype(std::ref(f)(std::forward<Args>(args)...)) {
            return std::ref(f)(std::forward<Args>(args)...);
        }
    }
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
    }
    #endif

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
        //if constexpr (sizeof...(S) > 0)
            return concat<concat_impl<S1, make_is<size(S1)>, S2, make_is<size(S2)>>::value, S...>();
        //else
        //    return concat_impl<S1, make_is<size(S1)>, S2, make_is<size(S2)>>::value;
    };

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
}

template<unsigned, typename...>struct log_stream_op;

template<unsigned S>struct log_stream_op<S> {
    const std::tuple<> args;
    constexpr log_stream_op() = default;
};


template<unsigned S, typename... Args>struct log_stream_op {
    const std::tuple<Args...> args;

    constexpr log_stream_op(std::tuple<Args...>&& args) : args{ std::move(args) } {}

    constexpr void log() const {
        #ifdef __cpp_lib_apply
          std::apply([this](auto&&... args) { std::printf(concat<format<Args>::fmt...>(), fix<Args>(args)...); }, args);
        #else
          detail::apply([this](auto&&... args) { std::printf(concat<format<Args>::fmt...>(), fix<Args>(args)...); }, args);
        #endif
    }
};

using error   = log_stream_op<LOG_ERR>;
using warning = log_stream_op<LOG_WARNING>;
using notice  = log_stream_op<LOG_NOTICE>;
using info    = log_stream_op<LOG_INFO>;
using debug   = log_stream_op<LOG_DEBUG>;
using trace   = log_stream_op<LOG_TRACE>;

template<unsigned S, typename... A, typename B>inline auto operator<<(log_stream_op<S, A...> &&a, const B &b) -> typename std::enable_if<!std::is_same<typename std::decay<B>::type, char*>::value  && !std::is_integral<B>::value && !std::is_floating_point<B>::value, log_stream_op<S, A..., std::string>>::type {
    std::ostringstream os;
    os << b;
    std::string s = os.str();
    return log_stream_op<S, A..., std::string>{ std::tuple_cat(a.args, std::tie(s)) };
}


template<unsigned S, typename... A>inline auto operator<<(log_stream_op<S, A...> &&a, const std::string &b) {
    return log_stream_op<S, A..., std::string>{ std::tuple_cat(a.args, std::tie(b)) };
}


template<unsigned S, typename... A, typename B>constexpr inline auto operator<<(log_stream_op<S, A...>&& a, const B b) -> typename std::enable_if<std::is_same<typename std::decay<B>::type, char*>::value || std::is_integral<B>::value || std::is_floating_point<B>::value, log_stream_op<S, A..., B>>::type {
    return log_stream_op<S, A..., B>{ std::tuple_cat(a.args, std::tie(b)) };
}

#ifdef USE_SYSLOG
  #undef  _____MAX
#endif

#endif  /* _ARROWHEAD_LOGGER_SYSLOG_H_ */
