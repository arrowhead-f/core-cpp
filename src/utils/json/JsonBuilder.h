#ifndef UTILS_JSONBUILDER_H_
#define UTILS_JSONBUILDER_H_

#include <sstream>
#include <string>
#include <type_traits>

namespace {

    template <typename T, typename = void>struct HasIsNull : std::false_type { };
    template <typename T> struct HasIsNull<T, std::void_t<decltype(T::is_null)>> : std::true_type {};

}


class JsonBuilder {

    private:

        std::ostringstream out;

        void write_val(const std::string &value) {
            out << "\"";
            for (size_t i = 0; i < value.length(); i++) {
                char ch = value[i];
                if (ch == '\\') {
                    out << "\\\\";
                }
                else if (ch == '"') {
                    out << "\\\"";
                }
                else if (ch == '\b') {
                    out << "\\b";
                }
                else if (ch == '\f') {
                    out << "\\f";
                }
                else if (ch == '\n') {
                    out << "\\n";
                }
                else if (ch == '\r') {
                    out << "\\r";
                }
                else if (ch == '\t') {
                    out << "\\t";
                }
                else if (static_cast<uint8_t>(ch) <= 0x1f) {
                    char buf[8];
                    snprintf(buf, sizeof buf, "\\u%04x", ch);
                    out << buf;
                }
                else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i + 1]) == 0x80 && static_cast<uint8_t>(value[i + 2]) == 0xa8) {
                    out << "\\u2028";
                    i += 2;
                }
                else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i + 1]) == 0x80 && static_cast<uint8_t>(value[i + 2]) == 0xa9) {
                    out << "\\u2029";
                    i += 2;
                }
                else {
                    out << ch;
                }
            }
            out << "\"";
        }

        auto write_val(bool value) {
           out << (value ? "true" : "false");
        }

        template<typename T>
        auto write_val(const T &value) -> typename std::enable_if<!std::is_arithmetic<T>::value, void>::type {
           out << "\"" << value << "\"";
        }

        template<typename T>
        auto write_val(T value) -> typename std::enable_if<std::is_arithmetic<T>::value, void>::type {
           out << value;
        }

    protected:

        auto write(const char *key, bool value) {
           out << '\"' << key << "\": " << (value ? "true" : "false") << ",";
        }


        template<typename T>
        auto write(const char *key, const T &value) -> typename std::enable_if<!std::is_arithmetic<T>::value, void>::type {
           out << '\"' << key << "\": \"" << value << "\",";
        }

        template<typename T>
        auto write(const char *key, T value) -> typename std::enable_if<std::is_arithmetic<T>::value, void>::type {
           out << '\"' << key << "\": " << value << ",";
        }

        auto write_null(const char *key) {
           out << '\"' << key << "\": null,";
        }

        void writeObj(const char *key, const std::string &value){
           out << '\"' << key << "\": " << value << ",";
        }

    public:

        JsonBuilder() : out("{") { out << "{"; }

        enum class ControlResp { USE, CONT, LAST, SKIP };


        #ifdef __GNUG__
          template<typename T>
        #else
          template<typename T, typename = std::void_t<decltype(T::key), decltype(T::value)>>
        #endif
        auto operator<<(const T &v) -> typename std::enable_if<!HasIsNull<T>::value, JsonBuilder&>::type {
            write(T::key, v.value);
            return *this;
        }

        #ifdef __GNUG__
          template<typename T>
        #else
          template<typename T, typename = std::void_t<decltype(T::key), decltype(T::value), decltype(T::is_null)>>
        #endif
        auto operator<<(const T &v) -> typename std::enable_if<HasIsNull<T>::value, JsonBuilder&>::type {
            if (v.is_null)
                write_null(T::key);
            else
                write(T::key, v.value);
            return *this;
        }

        std::string str() {
            out.seekp(-1, std::ios::end);
            out << "}";
            return out.str();
        }

        template<typename R, typename Fun>
        auto& to_array(const char *key, const R &row, Fun f) {
            out << "\"" << key << "\": [";
            if(row) {
                do {
                    f(out, row);
                    out << ",";
                } while(row->next());
                out.seekp(-1, std::ios::end);
            }
            out << "],";
            return *this;
        }

        template<typename InputIt, typename Fun>
        auto& to_array(const char *key, InputIt first, InputIt last, Fun f) {
            out << "\"" << key << "\": [";
            if (first != last) {
                for (; first != last; ++first) {
                    f(out, *first);
                    out << ",";
                }
                out.seekp(-1, std::ios::end);
            }
            out << "],";
            return *this;
        }

        template<typename InputIt>
        auto& to_array(const char *key, InputIt first, InputIt last) {
            out << "\"" << key << "\": [";
            if (first != last) {
                for (; first != last; ++first) {
                    write_val(*first);
                    out << ",";
                }
                out.seekp(-1, std::ios::end);
            }
            out << "],";
            return *this;
        }

        template<typename InputIt>
        auto& to_arrayObj(const char *key, InputIt first, InputIt last) {
            out << "\"" << key << "\": [";
            if (first != last) {
                for (; first != last; ++first) {
                    out << *first;
                    out << ",";
                }
                out.seekp(-1, std::ios::end);
            }
            out << "],";
            return *this;
        }

        template<typename R, typename Fun>
        auto for_each(const char *key, const R &row, Fun f) -> typename std::enable_if<!std::is_same<ControlResp, typename std::invoke_result<Fun, JsonBuilder&, const R&>::type>::value, JsonBuilder>::type& {
            out << "\"" << key << "\": [";
            if(row) {
                do {
                    out << "{";
                    f(*this, row);
                    out.seekp(-1, std::ios::end);
                    out << "},";
                } while(row->next());
                out.seekp(-1, std::ios::end);
            }
            out << "],";
            return *this;
        }


        template<typename R, typename Fun>
        auto for_each(const char *key, const R &row, Fun f) -> typename std::enable_if<std::is_same<ControlResp, typename std::invoke_result<Fun, JsonBuilder&, const R&>::type>::value, JsonBuilder>::type& {
            out << "\"" << key << "\": [";
            if(row) {
                do {
                    sta:

                    out << "{";
                    const auto res = f(*this, row);
                    out.seekp(-1, std::ios::end);

                    if (res != ControlResp::SKIP) {
                        out << "},";

                        switch (res) {
                            case ControlResp::USE:
                                goto sta;
                            case ControlResp::CONT:
                                break;
                            case ControlResp::LAST:
                            default:
                                goto sto;
                        }
                    }

                } while(row->next());

                sto:

                out.seekp(-1, std::ios::end);
            }
            out << "],";
            return *this;
        }

        template<typename InputIt, typename Fun>
        auto& for_each(const char *key, InputIt first, InputIt last, Fun f) {
            out << "\"" << key << "\": [";
            if (first != last) {
                for (; first != last; ++first) {
                    out << "{";
                    f(*this, *first);
                    out.seekp(-1, std::ios::end);
                    out << "},";
                }
                out.seekp(-1, std::ios::end);
            }
            out << "],";
            return *this;
        }

        template<typename R, typename Pred, typename Fun>
        auto take_while(const char *key, const R &row, Pred p, Fun f) {
            auto res = ControlResp::CONT;

            out << "\"" << key << "\": [";
            if(row) {
                do {
                    if (!p(row)) {
                        res = ControlResp::USE;
                        break;
                    }

                    out << "{";
                    f(*this, row);
                    out.seekp(-1, std::ios::end);
                    out << "},";

                } while(row->next());
                out.seekp(-1, std::ios::end);
            }
            out << "],";

            return res;
        }

        template<typename R, typename Pred, typename Fun>
        auto take_while_raw(const char *key, const R &row, Pred p, Fun f) {
            auto res = ControlResp::CONT;

            out << "\"" << key << "\": [";
            if(row) {
                do {
                    if (!p(row)) {
                        res = ControlResp::USE;
                        break;
                    }

                    f(out, row);

                } while(row->next());
                out.seekp(-1, std::ios::end);
            }
            out << "],";

            return res;
        }

        template<typename Row, typename H>
        auto write_dictionary_items(const Row &row, int i = 0) -> typename std::enable_if<!HasIsNull<H>::value && !std::is_same<decltype(H::value), bool>::value, void>::type {
            typename std::decay<decltype(H::value)>::type val;
            row->get(i, val);
            operator<<(H{ val });
        }

        template<typename Row, typename H>
        auto write_dictionary_items(const Row &row, int i = 0) -> typename std::enable_if<!HasIsNull<H>::value && std::is_same<decltype(H::value), bool>::value, void>::type {
            int val;
            row->get(i, val);
            operator<<(H{ val ? true : false });
        }


        template<typename Row, typename H>
        auto write_dictionary_items(const Row &row, int i = 0) -> typename std::enable_if<HasIsNull<H>::value && !std::is_same<decltype(H::value), bool>::value, void>::type {
            typename std::decay<decltype(H::value)>::type val;
            bool not_null = row->get(i, val);
            operator<<(H{ val, !not_null });
        }

        template<typename Row, typename H>
        auto write_dictionary_items(const Row &row, int i = 0) -> typename std::enable_if<HasIsNull<H>::value && std::is_same<decltype(H::value), bool>::value, void>::type {
            int val;
            bool not_null = row->get(i, val);
            operator<<(H{ val ? true : false, !not_null });
        }


        template<typename Row, typename H, typename ...T>
        auto write_dictionary_items(const Row &row, int i = 0) -> typename std::enable_if<(sizeof...(T) > 0), void>::type {
            write_dictionary_items<Row, H>(row, i);
            write_dictionary_items<Row, T...>(row, i + 1);
        }

        template<typename Row, typename H>
        void write_dictionary_items(const char *key, const Row &row, int i = 0) {
            out << "\"" << key << "\": {";
            write_dictionary_items<Row, H>(row, i);
            out.seekp(-1, std::ios::end);
            out << "},";
        }

        template<typename Row, typename H, typename ...T>
        auto write_dictionary_items(const char *key, const Row &row, int i = 0) -> typename std::enable_if<(sizeof...(T) > 0), void>::type {
            out << "\"" << key << "\": {";
            write_dictionary_items<Row, H>(row, i);
            write_dictionary_items<Row, T...>(row, i + 1);
            out.seekp(-1, std::ios::end);
            out << "},";
        }


        template<typename InputIt, typename Fun>
        auto write_dictionary_items(const char *key, InputIt first, InputIt last, Fun fun) {
            out << "\"" << key << "\": {";

            if (first != last) {
                for (; first != last; ++first) {
                    fun(out, *first);
                    out << ",";
                }
                out.seekp(-1, std::ios::end);
            }

            out.seekp(-1, std::ios::end);
            out << "},";
        }


};  // class JsonBuilder

#endif  /* UTILS_JSONBUILDER_H_ */
