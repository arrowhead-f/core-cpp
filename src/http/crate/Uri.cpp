#include "Uri.h"

#include <string>
#include <cstdlib>
#include <utility>


bool Uri::compare(const char *path, std::size_t N) noexcept {
    for (std::string::size_type i = 0; i < N; i++) {
        if (uri[p + i] != path[i]) {
            return false;
        }
    }

    if (uri.length() == p + N || uri[p + N] == '?') {
        p += N;
        return true;
    }

    return false;
}


bool Uri::compare(const std::string &path) noexcept {
    const auto N = path.length();
    for (std::string::size_type i = 0; i < N; i++) {
        if (uri[p + i] != path[i]) {
            return false;
        }
    }

    if (uri[p + N] == 0  || uri[p + N] == '?') {
        p += N;
        return true;
    }
    return false;
}


bool Uri::consume(const char *path, std::size_t N) noexcept {
    for (std::string::size_type i = 0; i < N; i++) {
        if (uri[p + i] != path[i]) {
            return false;
        }
    }

    if (uri.length() == p + N || uri[p + N] == '/' || uri[p + N] == '?') {
        p += N;
        return true;
    }

    return false;
}

bool Uri::consume(const std::string &path) noexcept {
    const auto N = path.length();
    for (std::string::size_type i = 0; i < N; i++) {
        if (uri[p + i] != path[i]) {
            return false;
        }
    }

    if (uri[p + N] == 0  || uri[p + N] == '/' || uri[p + N] == '?') {
        p += N;
        return true;
    }
    return false;
}


bool Uri::pathParam(std::string &result) const {
    if (uri[p] == '/') {
        const auto N = uri.length();
        auto i = p;
        for (; i < N; i++) {
            if (uri[i] == '?')
                break;
        }
        result = std::string{uri, p + 1, i};
        return true;
    }

    return false;
}


bool Uri::pathId(unsigned long &value) const noexcept {
    if (uri[p] == '/') {
        if (uri[p + 1] == '\0')
            return false;

        char *endp;
        value = std::strtoul(uri.c_str() + p + 1, &endp, 10);

        if (*endp)
            return false;
        return true;
    }

    return false;
}


bool Uri::pathId(int &value) const noexcept {
    if (uri[p] == '/') {
        if (uri[p + 1] == '\0')
            return false;

        value = std::stoi(uri.c_str() + p + 1);

        return true;
    }

    return false;
}


std::string Uri::Parser::urldecode(const char *s, std::size_t len) const noexcept {
    std::string result;
    for (std::string::size_type i = 0; i < len; ++i) {
        if (*(s + i) == '+') {
            result += ' ';
        }
        else if (*(s+ i) == '%' && len > i + 2) {
            const char ch1 = from_hex(*(s+i+1));
            const char ch2 = from_hex(*(s+i+2));
            const char ch = (ch1 << 4) | ch2;
            result += ch;
            i += 2;
        }
        else {
            result += *(s + i);
        }
    }
    return result;
}


Uri::Parser::Parser(const Uri &uri) : key{ uri.uri.c_str() + uri.p } {

    for (; *key; key++) {
        if (*key == '?')
            break;
    }

    if (!*key)
        return;

    for(val = key + 1; *val; val++) {
        if (*val == '=') {
            break;
        }
    }

    if (!*val)
        ok = false;
}


void Uri::Parser::next() noexcept {

    key = val + 1;
    for(; *key; key++) {
        if (*key == '&') {
            break;
        }
    }

    if (!*key) {
        return;
    }

    val = key + 1;
    for(; *val; val++) {
        if (*val == '=') {
            break;
        }
    }

    if (!*val) {
        ok = false;
        return;
    }
}


std::pair<std::string, std::string> Uri::Parser::operator*() const noexcept {
    std::pair<std::string, std::string> value;

    value.first = urldecode(key + 1, val - key - 1);

    const char *end = val;
    for(; *end; end++) {
        if (*end == '&')
            break;
    }

    value.second = urldecode(val + 1, end - val - 1);
    return value;
}


bool Uri::Parser::Sink::try_consume(const char *key, std::string &target, std::initializer_list<const char*> il) noexcept {
    if (kv.first.compare(key))
        return false;

    for(const auto &x : il) {
        if (!kv.second.compare(x)) {
            target = kv.second;
            return true;
        }
    }
    processing_error = true;
    return true;
}


bool Uri::Parser::Sink::try_consume_order(const char *key, std::string &target, std::initializer_list<const char*> il) noexcept {
    if (kv.first.compare(key))
        return false;

    char c = '0';
    for(const auto &x : il) {
        c++;
        if (!kv.second.compare(x)) {
            target = c;
            return true;
        }
    }
    processing_error = true;
    return true;
}
