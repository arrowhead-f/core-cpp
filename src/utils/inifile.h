#ifndef _UTILS_PARSERS_H_
#define _UTILS_PARSERS_H_


#include <fstream>
#include <map>
#include <stdexcept>
#include <string>


class INIFile {

    private:

        std::ifstream in;                           ///< The stream to read from.
        std::map<std::string, std::string> values;  ///< The stored values.

    public:

        /// Initialize without any file
        INIFile() = default;

        /// Ctor to create ay INI pfile parser.
        INIFile(const std::string &name) : in{ name } {
            if (!in)
                throw std::runtime_error{ "Cannot open INI file." };
        }

        /// Clear the given section. If nullptr given (this is the default), all the data is deleted.
        /// \param section      section to delete; use nullptr to delete everything
        void clear(const char *section = nullptr) {
            if (!section)
                values.clear();
            else
                values.erase(section);
        }

        /// Clears all the parsed/stored data and closes the actual file.
        void close() {
            values.clear();
            in.close();
        }

        /// Appends the given config string to the given section.
        /// \param section      the section to append to
        /// \param conf         configuation to append
        /// \return             itself to make the usage more fluent
        INIFile& append(const std::string &section, std::string conf) {
            auto &val = values[section];
            if (!val.empty())
                val.append(" ");
            val.append(std::move(conf));
            return *this;
        }

        /// Prepends the given config string to the given section.
        /// \param section      the section to append to
        /// \param conf         configuation to append
        /// \return             itself to make the usage more fluent
        INIFile& prepend(const std::string &section, std::string conf) {
            auto &val = values[section];
            if (!val.empty())
                val.insert(0, " ");
            val.insert(0, std::move(conf));
            return *this;
        }

        /// Internally swaps the config file. The data that was already parsed stays untouched.
        /// \param name         the name of the config file
        void load(const std::string &name) {
            in.close();
            in.open(name);
            if (!in)
                throw std::runtime_error{ "Cannot open INI file." };
        }

        /// Resets the parser. Sets the config file to the given one and deletes all the parsed data.
        /// \param name         the name of the config file
        void reset(const std::string &name) {
            load(name);
            clear(nullptr);
        }

        /// Parses the whole INI file.
        /// \return             whether the parsing was successful
        bool parse_all() {
            return parse(nullptr);
        }

        /// Overload of the parse method with std::string instead of const char*;
        /// \param section      the section to parse; use nullptr to request all the sections; use empty string for the noname section
        /// \param strict       if true, noname section is read together with the section given
        /// \return             whether the parsing was successful
        bool parse(const std::string &section, bool strict = false) {
            return parse(section.c_str(), strict);
        }

        /// Parses the file and reads the given group into the memory. If the section is nullptr, all the group all read.
        /// \param section      the section to parse; use nullptr to request all the sections; use empty string for the noname section
        /// \param strict       if true, noname section is read together with the section given
        /// \return             whether the parsing was successful
        bool parse(const char *section = nullptr, bool strict = false);

        /// Returns the raw string representation of the data found in the section.
        /// \param section      the name of the section; se empty string for the unnamed section
        /// \return             the unparsed config string for the section given
        const std::string& to_string(const std::string &section) const {
            static const std::string empty = "";

            const auto it = values.find(section);
            if (it != values.cend())
                return it->second;
            return empty;
        }

        /// Returns the options found in the given section.
        /// \param section      the section
        /// \return             the key-value map containing all the parsed entries in the section
        std::map<std::string, std::string> options(const std::string &section) {
            return parse_opt(values[section]);
        }

        /// Parses the options into a key-value map.
        /// \param opts         the options to parse
        /// \return             the opts parsed into a key-value map
        static std::map<std::string, std::string> parse_opt(const char *opts);

        /// Parses the options into a key-value map. Convenient function with std::string as param.
        /// \param opts         the options to parse
        /// \return             the opts parsed into a key-value map
        static std::map<std::string, std::string> parse_opt(const std::string &opts) {
            return parse_opt(opts.c_str());
        }

    };

#endif  /* _UTILS_PARSERS_H_ */
