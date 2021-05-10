#ifndef _URI_H_
#define _URI_H_


#include <string>


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


class Uri {

    private:

        std::string            uri;    ///< The necapsulated uri.
        std::string::size_type p = 0;  ///< The current reading position.

    public:

        Uri(const std::string &uri) : uri{ std::move(uri) } {}
        Uri(std::string &&uri) : uri{ std::move(uri) } {}

        bool empty() const noexcept {
            return (p == uri.length() || uri[p] == '?');
        }

        template<std::size_t N>
        bool compare(const char (&path)[N]) noexcept {
            return compare(path, N - 1);
        }

        bool compare(const std::string &path) noexcept;

        template<std::size_t N>
        bool consume(const char (&path)[N]) noexcept {
            return consume(path, N - 1);
        }

        bool consume(const std::string &path) noexcept;

        // create a string_view version of it
        bool pathParam(std::string&) const;

        bool pathId(unsigned long &value) const noexcept;

        const std::string& str() const noexcept {
            return uri;
        }


        /************************************************************************************/

        friend class Parser;

        class Parser {

            private:

                const char *key;  ///< The start of the parameter.
                const char *val;  ///< The start of the parameter.

                bool ok = true;   ///< To detect parsing errors.

                inline char from_hex(char ch) const noexcept {
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

                std::string urldecode(const char *s, std::size_t len) const noexcept;

                void next() noexcept;

            public:

                Parser(const Uri &uri);

                /// Move to the next key/valu pair.
                Parser& operator++() noexcept {
                    next();
                    return *this;
                }

                /// Check whether the next key-value pair can be retrieved.
                operator bool() const noexcept {
                    return ok && *key;
                }

                /// Returns the current key-value pair parsed.
                std::pair<std::string, std::string> operator*() const noexcept;

                /// Checks whether the parsing ended successfully.
                /// \return         the result of the parsing process
                bool check() const noexcept {
                    return ok;
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

                        bool try_consume(const char *key, std::string &target, std::initializer_list<const char*> il) noexcept;

                        bool try_consume_order(const char *key, std::string &target, std::initializer_list<const char*> il) noexcept;

                        bool failed() const noexcept {
                            return processing_error;
                        }

            };

        };


        /************************************************************************************/


    private:

        bool compare(const char *path, std::size_t N) noexcept;
        bool consume(const char *path, std::size_t N) noexcept;

};


#endif  /* _URI_H_ */
