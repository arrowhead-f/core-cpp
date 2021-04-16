#ifndef _HTTP_URLPARSER_H_
#define _HTTP_URLPARSER_H_


#include <cstddef>
#include <string>
#include <utility>


namespace http {

    namespace details {

        template<typename H>
        struct ValidatorValueConverter {
            static const bool convert(const std::string &val) noexcept {
                return true;
            }
        };

        template<>
        struct ValidatorValueConverter<int> {
            static bool convert(const std::string &val) noexcept {
                try { std::stoi(val); } catch(...) { return false; } return true;
            }
            static bool convert(const std::string &val, int &res) noexcept {
                try { res = std::stoi(val); } catch(...) { return false; } return true;
            }
        };

        template<>
        struct ValidatorValueConverter<long> {
            static bool convert(const std::string &val) noexcept {
                try { std::stol(val); } catch(...) { return false; } return true;
            }
            static bool convert(const std::string &val, long &res) noexcept {
                try { res = std::stol(val); } catch(...) { return false; } return true;
            }
        };

        template<>
        struct ValidatorValueConverter<unsigned> {
            static bool convert(const std::string &val) noexcept {
                try {
                    auto result = std::stoul(val);
                    if (result > std::numeric_limits<unsigned>::max())
                        return false;
                }
                catch(...) {
                    return false;
                }
                return true;
            }
            static bool convert(const std::string &val, unsigned &res) noexcept {
                try {
                    auto result = std::stoul(val);
                    if (result > std::numeric_limits<unsigned>::max())
                        return false;
                    res = result;
                }
                catch(...) {
                    return false;
                }
                return true;
            }
        };

        template<>
        struct ValidatorValueConverter<unsigned long> {
            static bool convert(const std::string &val) noexcept {
                try { std::stoul(val); } catch(...) { return false; } return true;
            }
            static bool convert(const std::string &val, unsigned long &res) noexcept {
                try { res = std::stoul(val); } catch(...) { return false; } return true;
            }
        };

        template<>
        struct ValidatorValueConverter<float> {
            static bool convert(const std::string &val) noexcept {
                try { std::stof(val); } catch(...) { return false; } return true;
            }
            static bool convert(const std::string &val, float &res) noexcept {
                try { res = std::stof(val); } catch(...) { return false; } return true;
            }
        };

        template<>
        struct ValidatorValueConverter<double> {
            static bool convert(const std::string &val) noexcept {
                try { std::stod(val); } catch(...) { return false; } return true;
            }
            static bool convert(const std::string &val, double &res) noexcept {
                try { res = std::stod(val); } catch(...) { return false; } return true;
            }
        };
    }


    struct UrlParser {

        private:

            const std::string &str;  ///< The url to be parsed.
            bool ok = true;          ///< To detect parsing errors.

            std::string::size_type path_len;  ///< The length of the path part.
            std::string::size_type key_pos;   ///< The position of the actual key.
            std::string::size_type val_pos;   ///< The position of the actual value.


            inline char from_hex(char ch) const {
                if (ch <= '9' && ch >= '0')
                    ch -= '0';
                else if (ch <= 'f' && ch >= 'a')
                    ch -= 'a' - 10;
                else if (ch <= 'F' && ch >= 'A')
                    ch -= 'A' - 10;
                else 
                    ch = 0;
                return ch;
            }

            std::string urldecode(const char *s, std::size_t len) const {
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

            void next() {
                std::string::size_type i = val_pos;
                for(; i < str.length(); ++i) {
                    if (str[i] == '&') {
                        break;
                    }
                }

                if (i == str.length()) {
                    key_pos = std::string::npos;
                    return;
                }
                else
                    key_pos = i;

                std::string::size_type j = key_pos;
                for(; j < str.length(); ++j) {
                    if (str[j] == '=') {
                        break;
                    }
                }

                if (j == str.length())
                    ok = false;

                val_pos = j;

                if (val_pos - key_pos - 1 == 0)
                    ok = false;
            }

        public:

            UrlParser(const std::string &str) : str(str), path_len{ str.length() }, key_pos{ std::string::npos }, val_pos{ std::string::npos } {
                for(std::string::size_type i = 0; i < str.length(); ++i) {
                    if (str[i] == '?') {
                        path_len = key_pos = i;
                        break;
                    }
                }

                if (key_pos == std::string::npos)
                    return;

                for(std::string::size_type i = key_pos; i < str.length(); ++i) {
                    if (str[i] == '=') {
                        val_pos = i;
                        break;
                    }
                }

                if (val_pos == std::string::npos)
                    ok = false;
            }

            /// Move to the next key/valu pair.
            UrlParser& operator++() {
                next();
                return *this;
            }

            /// Check whether the next key-value pair can be retrieved.
            operator bool() {
                return ok && (key_pos != std::string::npos);
            }

            /// Returns the current key-value pair parsed.
            auto operator*() const {
                std::pair<std::string, std::string> value;

                value.first = urldecode(&str[key_pos + 1], val_pos - key_pos - 1);

                std::string::size_type i = val_pos;
                for(; i < str.length(); ++i) {
                    if (str[i] == '&')
                        break;
                }

                value.second = urldecode(&str[val_pos + 1] , i - val_pos - 1);
                return value;
            }

            /// Checks whether the parsing ended successfully.
            /// \return         the result of the parsing process
            bool check() const {
                return ok;
            }

            /// Return the path part of the url.
            /// \return         the path url decoded
            auto getPath() const {
                return urldecode(str.c_str(), path_len);
            }


            class Sink {

                private:

                    const std::pair<std::string, std::string> &kv;
                    bool processing_error = false;

                public:

                    Sink(const std::pair<std::string, std::string> &kv) : kv{ kv }{}

                    bool try_consume(const char *key, std::string &target) noexcept {
                        if (kv.first.compare(key))
                            return false;

                        target = kv.second;
                        return true;
                    }

                    template<typename T>
                    auto try_consume(const char *key, T &target) noexcept -> typename std::enable_if<std::is_same<T, int>::value || std::is_same<T, long>::value || std::is_same<T, unsigned>::value || std::is_same<T, unsigned long>::value || std::is_same<T, float>::value || std::is_same<T, double>::value, bool>::type {
                        if (kv.first.compare(key))
                            return false;

                        if (!details::ValidatorValueConverter<T>::convert(kv.second, target)) processing_error = true;
                        return true;
                    }

                    template<typename T>
                    bool try_consume_as(const char *key, std::string &target) noexcept {
                        if (kv.first.compare(key))
                            return false;

                        if (details::ValidatorValueConverter<T>::convert(kv.second)) target = kv.second; else processing_error = true;
                        return true;
                    }

                    bool try_consume(const char *key, std::string &target, std::initializer_list<const char*> il) noexcept {
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

                    bool try_consume_order(const char *key, std::string &target, std::initializer_list<const char*> il) noexcept {
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

                    bool failed() const {
                        return processing_error;
                    }

        };



    };


}

#endif  /* _HTTP_URLPARSER_H_ */
