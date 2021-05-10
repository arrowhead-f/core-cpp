#ifndef _JSON_H_
#define _JSON_H_

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

        template<typename T>
        auto write_val(const T &value) -> typename std::enable_if<!std::is_arithmetic<T>::value, void>::type {
           out << "\"" << value << "\"";
        }

        template<typename T>
        auto write_val(T value) -> typename std::enable_if<std::is_arithmetic<T>::value, void>::type {
           out << value;
        }

    protected:

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
        auto for_each(const char *key, const R &row, Fun f) -> typename std::enable_if<!std::is_same<bool, typename std::invoke_result<Fun, JsonBuilder&, const R&>::type>::value, JsonBuilder>::type& {
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
        auto for_each(const char *key, const R &row, Fun f) -> typename std::enable_if<std::is_same<bool, typename std::invoke_result<Fun, JsonBuilder&, const R&>::type>::value, JsonBuilder>::type& {
            out << "\"" << key << "\": [";
            if(row) {
                do {
                    out << "{";
                    const auto res = f(*this, row);
                    out.seekp(-1, std::ios::end);
                    if (res)
                        out << "},";
                } while(row->next());
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

        template<typename Row, typename H>
        auto write_dictionary_items(const Row &row, int i = 0) -> typename std::enable_if<!HasIsNull<H>::value, void>::type {
            typename std::decay<decltype(H::value)>::type val;
            row->get(i, val);
            operator<<(H{ val });
        }

        template<typename Row, typename H>
        auto write_dictionary_items(const Row &row, int i = 0) -> typename std::enable_if<HasIsNull<H>::value, void>::type {
            typename std::decay<decltype(H::value)>::type val;
            bool not_null = row->get(i, val);
            operator<<(H{ val, !not_null });
        }

        template<typename Row, typename H, typename ...T>
        auto write_dictionary_items(const Row &row, int i = 0) -> typename std::enable_if<(sizeof...(T) > 0), void>::type {
            write_dictionary_items<Row, H>(row, i);
            write_dictionary_items<Row, T...>(row, i + 1);
        }

        template<typename Row, typename H, typename ...T>
        auto write_dictionary_items(const char *key, const Row &row, int i = 0) -> typename std::enable_if<(sizeof...(T) > 0), void>::type {
            out << "\"" << key << "\": {";
            write_dictionary_items<Row, H>(row, i);
            write_dictionary_items<Row, T...>(row, i + 1);
            out.seekp(-1, std::ios::end);
            out << "},";
        }

};

#endif  /* _JSON_H_ */
