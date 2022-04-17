#ifndef UTILS_JSONCVIEW_H_
#define UTILS_JSONCVIEW_H_


#include <cassert>
#include <cmath>
#include <cstring>
#include <exception>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


class JsonCView {

    private:

        static constexpr int max_depth = 200;      ///< The max recursion depth
        char *data                     = nullptr;  ///< The parsed data

    public:

        class JsonNode {

            public:

                class JsonArray : private std::vector<JsonNode> {
                    private:

                        using Parent = std::vector<JsonNode>;

                        friend class JsonCView;
                        friend struct JsonParser;

                        void insert(JsonNode &&other) { Parent::push_back(std::move(other)); }
                        template<typename ...Args>void emplace(Args&& ...args) { Parent::emplace_back(std::forward<Args>(args)...); }

                    public:

                        const JsonNode& at(std::size_t ith) const { return Parent::at(ith); }
                        JsonNode& at(std::size_t ith) { return Parent::at(ith); }

                        void erase(std::size_t ith) { if (ith < size()) Parent::erase(Parent::begin() + ith); }

                        std::size_t size() const noexcept { return Parent::size(); }

                        auto begin() noexcept { return Parent::begin(); }
                        auto end()   noexcept { return Parent::end(); }

                        auto begin() const noexcept { return Parent::begin(); }
                        auto end()   const noexcept { return Parent::end(); }

                        auto cbegin() const noexcept { return Parent::cbegin(); }
                        auto cend()   const noexcept { return Parent::cend(); }
                };


                class JsonObject : private std::vector<std::pair<char const*, JsonNode>> {
                    private:

                        using Parent = std::vector<std::pair<char const*, JsonNode>>;

                        friend class JsonCView;
                        friend struct JsonParser;

                        void insert(const char *key, JsonNode &&other) { Parent::push_back(std::make_pair(key, std::move(other))); }

                    public:
                        JsonObject() = default;

                        auto find(const char *key) const {
                            return std::find_if(Parent::cbegin(), Parent::cend(), [&key](const auto &kv) { return std::strcmp(kv.first, key) == 0; });
                        }

                        const JsonNode& at(const char *key) const {
                            if (const auto it = find(key); it != Parent::cend())
                                return it->second;
                            throw std::runtime_error{ "Not found." };
                        }

                        JsonNode& at(const char *key) {
                            auto it = std::find_if(Parent::begin(), Parent::end(), [&key](const auto &kv) { return std::strcmp(kv.first, key) == 0; });
                            if (it != Parent::cend())
                                return it->second;
                            throw std::runtime_error{ "Not found." };
                        }

                        void erase(const char *key) {
                            if (const auto it = find(key); it != Parent::cend())
                                Parent::erase(it);
                        }

                        std::size_t size() const noexcept { return Parent::size(); }

                        auto begin() noexcept { return Parent::begin(); }
                        auto end()   noexcept { return Parent::end(); }

                        auto begin() const noexcept { return Parent::begin(); }
                        auto end()   const noexcept { return Parent::end(); }

                        auto cbegin() const noexcept { return Parent::cbegin(); }
                        auto cend()   const noexcept { return Parent::cend(); }
                };

            private:

                std::variant<std::nullptr_t, bool, long, double, std::string*, JsonArray*, JsonObject*> data;

                static const JsonNode& static_null() {
                    static const JsonNode json_null;
                    return json_null;
                }

            public:

                JsonNode(const JsonNode &other) : data{ other.data } {
                    if (std::holds_alternative<std::string*>(other.data)) {
                        data = new std::string{ *std::get<std::string*>(other.data) };
                        return;
                    }
                    if (std::holds_alternative<JsonArray*>(other.data)) {
                        data = new JsonArray{ *std::get<JsonArray*>(other.data) };
                        return;
                    }
                    if (std::holds_alternative<JsonObject*>(other.data)) {
                        data = new JsonObject{ *std::get<JsonObject*>(other.data) };
                        return;
                    }
                }

                JsonNode& operator=(const JsonNode &other) {
                    if (this != std::addressof(other)) {
                        if (std::holds_alternative<std::string*>(other.data)) {
                            delete std::get<std::string*>(other.data);
                            data = new std::string{ *std::get<std::string*>(other.data) };

                            return *this;
                        }
                        if (std::holds_alternative<JsonArray*>(other.data)) {
                            delete std::get<JsonArray*>(other.data);
                            data = new JsonArray{ *std::get<JsonArray*>(other.data) };

                            return *this;
                        }
                        if (std::holds_alternative<JsonObject*>(other.data)) {
                            delete std::get<JsonObject*>(other.data);
                            data = new JsonObject{ *std::get<JsonObject*>(other.data) };

                            return *this;
                        }

                        data = other.data;
                    }

                    return *this;
                }

                JsonNode(JsonNode &&other) noexcept : data{ std::move(other.data) } {
                    if (std::holds_alternative<std::string*>(other.data) || std::holds_alternative<JsonArray*>(other.data) || std::holds_alternative<JsonObject*>(other.data))
                        other.data = nullptr;
                }

                JsonNode& operator=(JsonNode && other) noexcept {
                    std::swap(data, other.data);
                    return *this;
                }

                ~JsonNode() {
                    if (std::holds_alternative<std::string*>(data))
                        delete std::get<std::string*>(data);
                    else if (std::holds_alternative<JsonArray*>(data))
                        delete std::get<JsonArray*>(data);
                    else if (std::holds_alternative<JsonObject*>(data))
                        delete std::get<JsonObject*>(data);
                }

            private:

                friend class JsonCView;
                friend struct JsonParser;

                explicit JsonNode() noexcept : data{ nullptr } {}
                explicit JsonNode(std::nullptr_t) noexcept : data{ nullptr } {}

                explicit JsonNode(double value) : data{ value } {}
                explicit JsonNode(int value) : data{ static_cast<long>(value) } {}
                explicit JsonNode(long value) : data{ value } {}
                explicit JsonNode(bool value) : data{ value } {}

                explicit JsonNode(const std::string &value) : data{ new std::string{ value } } {}
                explicit JsonNode(std::string &&value) : data{ new std::string{ std::move(value) } } {}
                explicit JsonNode(std::string *value) : data{ value } {}

                explicit JsonNode(const JsonArray &values) : data{ new JsonArray{ values } } {}
                explicit JsonNode(JsonArray &&values) : data{ new JsonArray{ std::move(values) } } {}
                explicit JsonNode(const JsonObject &values) : data{ new JsonObject{ values} } {}
                explicit JsonNode(JsonObject &&values) : data{ new JsonObject{ std::move(values) } } {}

            public:

                bool is_null()   const noexcept { return std::holds_alternative<std::nullptr_t>(data); }
                bool is_int()    const noexcept { return std::holds_alternative<long>(data); }
                bool is_double() const noexcept { return std::holds_alternative<double>(data); }
                bool is_bool()   const noexcept { return std::holds_alternative<bool>(data); }
                bool is_number() const noexcept { return std::holds_alternative<double>(data) || std::holds_alternative<long>(data); }
                bool is_string() const noexcept { return std::holds_alternative<std::string*>(data); }
                bool is_array()  const noexcept { return std::holds_alternative<JsonArray*>(data); }
                bool is_object() const noexcept { return std::holds_alternative<JsonObject*>(data); }

                double as_number() const noexcept { return std::holds_alternative<double>(data) ? std::get<double>(data) : std::get<long>(data); }

                long  as_int() const { return std::get<long>(data); }
                long& as_int() { return std::get<long>(data); }

                double  as_double() const { return std::get<double>(data); }
                double& as_double() { return std::get<double>(data); }

                bool  as_bool() const { return std::get<bool>(data); }
                bool& as_bool() { return std::get<bool>(data); }

                const std::string& as_string() const { return *std::get<std::string*>(data); }
                std::string&  as_string() { return *std::get<std::string*>(data); }

                const JsonArray& as_array() const { return *std::get<JsonArray*>(data); }
                JsonArray& as_array() { return *std::get<JsonArray*>(data); }

                const JsonObject& as_object() const { return *std::get<JsonObject*>(data); }
                JsonObject& as_object() { return *std::get<JsonObject*>(data); }

                const JsonNode& value_or_null(size_t i) const noexcept {
                    const auto &a = *std::get<JsonArray*>(data);
                    if (i > a.size())
                        return static_null();
                    return a.at(i);
                }

                const JsonNode& at(size_t i) const { return const_cast<const JsonArray*>(std::get<JsonArray*>(data))->at(i); }
                JsonNode& at(size_t i) { return std::get<JsonArray*>(data)->at(i); }

                const JsonNode& value_or_null(const char *key) const noexcept {
                    const auto &o = *std::get<JsonObject*>(data);
                    const auto it = o.find(key);

                    if (it == o.cend())
                        return static_null();

                    return o.at(key);
                }

                const JsonNode& at(const char *key) const { return const_cast<const JsonObject*>(std::get<JsonObject*>(data))->at(key); }         
                JsonNode& at(const char *key) { return std::get<JsonObject*>(data)->at(key); }

                std::string dump() const {
                    std::string out;
                    dump(out);
                    return out;
                }

            private:

                struct JsonNodeVisitor {
                    std::string &out;
                    JsonNodeVisitor(std::string &out) : out{ out } {}
                    void operator()(std::nullptr_t) const { out += "null"; }
                    void operator()(long v) const { out += std::to_string(v); }
                    void operator()(double v) const { out += std::to_string(v); }
                    void operator()(bool v) const { out += v ? "true" : "false"; }

                    void operator()(const std::string *v) const { 
                        const std::string &value = *v;
                        out += '"';
                        for (size_t i = 0; i < value.length(); i++) {
                            const char ch = value[i];
                            if (ch == '\\') {
                                out += "\\\\";
                            }
                            else if (ch == '"') {
                                out += "\\\"";
                            }
                            else if (ch == '\b') {
                                out += "\\b";
                            }
                            else if (ch == '\f') {
                                out += "\\f";
                            }
                            else if (ch == '\n') {
                                out += "\\n";
                            }
                            else if (ch == '\r') {
                                out += "\\r";
                            }
                            else if (ch == '\t') {
                                out += "\\t";
                            }
                            else if (static_cast<uint8_t>(ch) <= 0x1f) {
                                char buf[8];
                                snprintf(buf, sizeof buf, "\\u%04x", ch);
                                out += buf;
                            }
                            else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i + 1]) == 0x80
                                && static_cast<uint8_t>(value[i + 2]) == 0xa8) {
                                out += "\\u2028";
                                i += 2;
                            }
                            else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i + 1]) == 0x80
                                && static_cast<uint8_t>(value[i + 2]) == 0xa9) {
                                out += "\\u2029";
                                i += 2;
                            }
                            else {
                                out += ch;
                            }
                        }
                        out += '"';
                    }

                    void operator()(const JsonArray *a) const {
                        bool first = true;
                        out += "[";
                        for (auto it = a->cbegin(); it != a->cend(); ++it) {
                            if (!first)
                                out += ", ";
                            it->dump(out);
                            first = false;
                        }
                        out += "]";

                    }

                    void operator()(const JsonObject *o) const {
                        bool first = true;
                        out += "{";
                        for (auto it = o->cbegin(); it != o->cend(); ++it) {
                            if (!first)
                                out += ", ";
                            out += std::string{ "\"" } + std::string{ it->first } + "\":";
                            it->second.dump(out);
                            first = false;
                        }
                        out += "}";

                    }

                };

                void dump(std::string &out) const {
                    std::visit(JsonNodeVisitor{ out }, data);
                }

        };

    private:

        JsonNode r;  ///< The root node.

    public:

        using StringType = char const*;

        enum ParseMethod { STANDARD, NOTHROW, COMMENTS };

        JsonCView(char *data, ParseMethod method = STANDARD) : data{ data }, r{ method == NOTHROW ? parse(STANDARD) : parse_throw_on_error(method) } { }

        JsonCView(const JsonCView&) = delete;
        JsonCView& operator=(const JsonCView&) = delete;

        JsonCView(JsonCView&&) = default;
        JsonCView& operator=(JsonCView&&) = default;

        const JsonNode& root() const & noexcept { return r; }
        JsonNode& root() & noexcept { return r; }

        std::string dump() const {
            return r.dump();
        }

    private:

        struct JsonParser {

            private:

                char *str;                     ///< The string to parse.
                std::size_t N;                 ///< Length of the string.
                std::size_t i = 0;             ///< The current position during the parsing.

                const ParseMethod strategy;    ///< The strategy used for parsing.

                bool failed = false;  ///< Whether the parsing failed or not.
                std::string err;      ///< The string containing the error message.

            public:

                JsonParser(char *str, std::size_t N, ParseMethod strategy) : str{ str }, N{ N }, strategy{ strategy } {}
                JsonParser(char *str, ParseMethod strategy) : str{ str }, N{ std::strlen(str) }, strategy{ strategy } {}

                JsonNode parse() { JsonNode res = parse_json(0); flush(); return res; }

                bool good() const { return !failed; }
                bool eof() const { return i == N; }

                void flush() { consume_garbage(); }

                const std::string& what() const { return err; }

            private:

                static char* slice(char *start, std::size_t len) {
                    *(start + len) = 0;
                    return start;
                }

                static bool in_range(long x, long lower, long upper) {
                    return (x >= lower && x <= upper);
                }

                static std::string esc(char c) {
                    if (static_cast<uint8_t>(c) >= 0x20 && static_cast<uint8_t>(c) <= 0x7f)
                        return "'" + std::string(1, c) + std::string{ "' (" } + std::to_string(static_cast<uint8_t>(c)) + ")";
                    else
                        return "(" + std::to_string(static_cast<uint8_t>(c)) + ")";
                }

                JsonNode fail(std::string &&msg) {
                    return fail(std::move(msg), JsonNode{});
                }

                template <typename T>
                T fail(std::string &&msg, T err_ret) {
                    if (!failed)
                        err = std::move(msg);
                    failed = true;
                    return std::move(err_ret);
                }

                void consume_whitespace() {
                    while (str[i] == ' ' || str[i] == '\r' || str[i] == '\n' || str[i] == '\t')
                        i++;
                }

                bool consume_comment() {
                    bool comment_found = false;
                    if (str[i] == '/') {
                        i++;

                        if (i == N)
                            return fail("Unexpected end of input after start of comment.", false);

                        if (str[i] == '/') { // inline comment
                            i++;
                            while (i < N && str[i] != '\n')
                                i++;
                            comment_found = true;
                        }
                        else if (str[i] == '*') { // multiline comment
                            i++;
                            if (i > N - 2)
                                return fail("Unexpected end of input inside multi-line comment.", false);
                            while (!(str[i] == '*' && str[i + 1] == '/')) {
                                i++;
                                if (i > N - 2)
                                    return fail("Unexpected end of input inside multi-line comment.", false);
                            }
                            i += 2;
                            comment_found = true;
                        }
                        else
                            return fail("Malformed comment.", false);
                    }
                    return comment_found;
                }

                void consume_garbage() {
                    consume_whitespace();
                    if (strategy == COMMENTS) {
                        bool comment_found = false;
                        do {
                            comment_found = consume_comment();
                            if (failed) return;
                            consume_whitespace();
                        } while (comment_found);
                    }
                }

                char get_next_token() {
                    consume_garbage();
                    if (failed) return static_cast<char>(0);
                    if (i == N)
                        return fail("Unexpected end of input.", static_cast<char>(0));
                    return str[i++];
                }

                char* parse_key() {
                    std::size_t start = i;
                    std::size_t cnt = 0;

                    while (true) {

                        if (i == N)
                            return fail("Unexpected end of input in string.", slice(str + start, 0));

                        char ch = str[i++];

                        if (ch == '"') {
                            return slice(str + start, i - start - 1);
                        }

                        // The usual case: non-escaped characters
                        if (ch != '\\') {
                            continue;
                        }

                        // Handle escapes
                        if (i == N)
                            return fail("Unexpected end of input in string.", slice(str + start, 0));

                        ch = str[i++];

                        if (ch == 'u') {
                            if (i + 4 > N)
                                return fail("Invalid escape character " + esc(ch) + ".", slice(str + start, 0));

                            i += 4;
                            continue;
                        }

                        if (ch != 'b' && ch != 'f' && ch != 'n' && ch != 'r' && ch != 't' && ch != '"' && ch != '\\' && ch != '/')
                            return fail("Invalid escape character " + esc(ch) + ".", slice(str + start, 0));
                    }
                }

                static void encode_utf8(long pt, std::string &out) {
                    if (pt < 0)
                        return;

                    if (pt < 0x80) {
                        out += static_cast<char>(pt);
                    }
                    else if (pt < 0x800) {
                        out += static_cast<char>((pt >> 6) | 0xC0);
                        out += static_cast<char>((pt & 0x3F) | 0x80);
                    }
                    else if (pt < 0x10000) {
                        out += static_cast<char>((pt >> 12) | 0xE0);
                        out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
                        out += static_cast<char>((pt & 0x3F) | 0x80);
                    }
                    else {
                        out += static_cast<char>((pt >> 18) | 0xF0);
                        out += static_cast<char>(((pt >> 12) & 0x3F) | 0x80);
                        out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
                        out += static_cast<char>((pt & 0x3F) | 0x80);
                    }
                }

                std::string parse_string() {
                    std::string out;
                    long last_escaped_codepoint = -1;
                    while (true) {
                        if (i == N)
                            return fail("Unexpected end of input in string.", "");

                        char ch = str[i++];

                        if (ch == '"') {
                            encode_utf8(last_escaped_codepoint, out);
                            return out;
                        }

                        if (in_range(ch, 0, 0x1f))
                            return fail("Unescaped " + esc(ch) + " in string.", "");

                        // The usual case: non-escaped characters
                        if (ch != '\\') {
                            encode_utf8(last_escaped_codepoint, out);
                            last_escaped_codepoint = -1;
                            out += ch;
                            continue;
                        }

                        // Handle escapes
                        if (i == N)
                            return fail("Unexpected end of input in string.", "");

                        ch = str[i++];

                        if (ch == 'u') {
                            std::string esc = std::string{ str + i, str + i + 4 };
                            if (esc.length() < 4) {
                                return fail("Bad \\u escape: " + esc + ".", "");
                            }
                            for (size_t j = 0; j < 4; j++) {
                                if (!in_range(esc[j], 'a', 'f') && !in_range(esc[j], 'A', 'F')
                                    && !in_range(esc[j], '0', '9'))
                                    return fail("Bad \\u escape: " + esc + ".", "");
                            }

                            long codepoint = strtol(esc.data(), nullptr, 16);

                            if (in_range(last_escaped_codepoint, 0xD800, 0xDBFF) && in_range(codepoint, 0xDC00, 0xDFFF)) {
                                encode_utf8((((last_escaped_codepoint - 0xD800) << 10)
                                    | (codepoint - 0xDC00)) + 0x10000, out);
                                last_escaped_codepoint = -1;
                            }
                            else {
                                encode_utf8(last_escaped_codepoint, out);
                                last_escaped_codepoint = codepoint;
                            }

                            i += 4;
                            continue;
                        }

                        encode_utf8(last_escaped_codepoint, out);
                        last_escaped_codepoint = -1;

                        if (ch == 'b') {
                            out += '\b';
                        }
                        else if (ch == 'f') {
                            out += '\f';
                        }
                        else if (ch == 'n') {
                            out += '\n';
                        }
                        else if (ch == 'r') {
                            out += '\r';
                        }
                        else if (ch == 't') {
                            out += '\t';
                        }
                        else if (ch == '"' || ch == '\\' || ch == '/') {
                            out += ch;
                        }
                        else {
                            return fail("Invalid escape character " + esc(ch) + ".", "");
                        }
                    }
                }

                JsonNode parse_number() {
                    size_t start_pos = i;

                    if (str[i] == '-')
                        i++;

                    // Integer part
                    if (str[i] == '0') {
                        i++;
                        if (in_range(str[i], '0', '9'))
                            return fail("Leading zeroes are not permitted in numbers.");
                    }
                    else if (in_range(str[i], '1', '9')) {
                        i++;
                        while (in_range(str[i], '0', '9'))
                            i++;
                    }
                    else {
                        return fail("Invalid " + esc(str[i]) + " in number.");
                    }

                    if (str[i] != '.' && str[i] != 'e' && str[i] != 'E' && (i - start_pos) <= static_cast<size_t>(std::numeric_limits<int>::digits10)) {
                        return JsonNode{ std::atol(str + start_pos) };
                    }

                    // Decimal part
                    if (str[i] == '.') {
                        i++;
                        if (!in_range(str[i], '0', '9'))
                            return fail("At least one digit required in fractional part.");

                        while (in_range(str[i], '0', '9'))
                            i++;
                    }

                    // Exponent part
                    if (str[i] == 'e' || str[i] == 'E') {
                        i++;

                        if (str[i] == '+' || str[i] == '-')
                            i++;

                        if (!in_range(str[i], '0', '9'))
                            return fail("At least one digit required in exponent.");

                        while (in_range(str[i], '0', '9'))
                            i++;
                    }

                    return JsonNode{ std::strtod(str + start_pos, nullptr) };
                }

                JsonNode expect(const std::string &expected, JsonNode res) {
                    assert(i != 0);
                    i--;
                    if (std::strncmp(str + i, expected.c_str(), expected.length()) == 0) {
                        i += expected.length();
                        return res;
                    }
                    else {
                        return fail("Expected " + expected + ", got " + std::string{ str }.substr(i, expected.length()));
                    }
                }

                JsonNode parse_json(int depth) {

                    if (depth > max_depth)
                        return fail("Exceeded maximum nesting depth.");

                    char ch = get_next_token();
                    if (failed)
                        return JsonNode{};

                    if (ch == '-' || (ch >= '0' && ch <= '9')) {
                        i--;
                        return parse_number();
                    }

                    if (ch == 't')
                        return expect("true", JsonNode{ true });

                    if (ch == 'f')
                        return expect("false", JsonNode{ false });

                    if (ch == 'n')
                        return expect("null", JsonNode{});

                    if (ch == '"')
                        return JsonNode{ parse_string() };

                    if (ch == '{') {
                        JsonNode::JsonObject data;
                        ch = get_next_token();
                        if (ch == '}')
                            return JsonNode{ std::move(data) };

                        while (1) {
                            if (ch != '"')
                                return fail("Expected '\"' in object, got " + esc(ch) + ".");

                            const char *key = parse_key();
                            if (failed)
                                return JsonNode{};

                            ch = get_next_token();
                            if (ch != ':')
                                return fail("Expected ':' in object, got " + esc(ch) + ".");

                            auto val = parse_json(depth + 1);

                            if (failed)
                                return JsonNode{};

                            data.insert(key, std::move(val));

                            ch = get_next_token();
                            if (ch == '}')
                                break;
                            if (ch != ',')
                                return fail("Expected ',' in object, got " + esc(ch) + ".");

                            ch = get_next_token();
                        }
                        return JsonNode{ std::move(data) };
                    }

                    if (ch == '[') {
                        JsonNode::JsonArray data;
                        ch = get_next_token();
                        if (ch == ']')
                            return JsonNode{ std::move(data) };

                        while (1) {
                            i--;
                            data.insert(parse_json(depth + 1));
                            if (failed)
                                return JsonNode();

                            ch = get_next_token();
                            if (ch == ']')
                                break;
                            if (ch != ',')
                                return fail("Expected ',' in list, got " + esc(ch) + ".");

                            ch = get_next_token();
                            (void)ch;
                        }
                        return JsonNode{ std::move(data) };
                    }

                    return fail("Expected value, got " + esc(ch) + ".");
                }
        };

        JsonNode parse_throw_on_error(ParseMethod strategy) {

            JsonParser parser{ data, strategy };
            JsonNode result = parser.parse();

            if (!parser.good())
                throw std::runtime_error{ parser.what() };
            if (!parser.eof())
                throw std::runtime_error{ "Unexpected trailing." };

            return result;
        }

        JsonNode parse(ParseMethod strategy) noexcept {

            JsonParser parser{ data, strategy };
            JsonNode result = parser.parse();

            if (!parser.good())
                return JsonNode{};
            if (!parser.eof())
                return JsonNode{};

            return result;
        }

    };  // class JsonCView

#endif  /* UTILS_JSONCVIEW_H_ */
