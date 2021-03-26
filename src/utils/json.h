#ifndef _JSON_H_
#define _JSON_H_

#include <sstream>
#include <string>
#include <type_traits>

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
        auto& operator<<(const T &v) {
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
        auto& for_each(const char *key, const R &row, Fun f) {
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

};

#endif  /* _JSON_H_ */
