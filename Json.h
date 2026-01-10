#pragma once

namespace nlohmann {
    class json;
}

namespace nlohmann {
    class json_exception : public std::exception {
    protected:
        std::string m_message;
    public:
        json_exception(const std::string& msg) : m_message(msg) {}
        virtual const char* what() const throw() {
            return m_message.c_str();
        }
        virtual ~json_exception() throw() {}
    };

    class parse_error : public json_exception {
    public:
        parse_error(const std::string& msg) : json_exception(msg) {}
    };

    class type_error : public json_exception {
    public:
        type_error(const std::string& msg) : json_exception(msg) {}
    };

    class out_of_range : public json_exception {
    public:
        out_of_range(const std::string& msg) : json_exception(msg) {}
    };

    class json {
    public:
        // Type definitions
        enum value_t {
            null,
            object,
            array,
            string,
            boolean,
            number_integer,
            number_float
        };

        typedef std::map<std::string, json>::iterator iterator;
        typedef std::map<std::string, json>::const_iterator const_iterator;

        // Constructors
        json() : m_type(null), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            m_value.number_integer = 0;
        }

        json(void* null_ptr) : m_type(null), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            (void)null_ptr;
            m_value.number_integer = 0;
        }

        json(bool val) : m_type(boolean), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            m_value.boolean = val;
        }

        json(int val) : m_type(number_integer), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            m_value.number_integer = static_cast<long long>(val);
        }

        json(long long val) : m_type(number_integer), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            m_value.number_integer = val;
        }

        json(double val) : m_type(number_float), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            m_value.number_float = val;
        }

        json(const std::string& val) : m_type(string), m_object(nullptr), m_array(nullptr) {
            m_string = new std::string(val);
            m_value.number_integer = 0;
        }

        json(const char* val) : m_type(string), m_object(nullptr), m_array(nullptr) {
            m_string = new std::string(val);
            m_value.number_integer = 0;
        }

        json(unsigned int val) : m_type(number_integer), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            m_value.number_integer = static_cast<long long>(val);
        }

        json(unsigned long long val) : m_type(number_integer), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            m_value.number_integer = static_cast<long long>(val);
        }

        // Copy constructor
        json(const json& other) : m_type(other.m_type), m_string(nullptr), m_object(nullptr), m_array(nullptr) {
            copy_from(other);
        }

        // Destructor
        ~json() {
            clear();
        }

        // Assignment operator
        json& operator=(const json& other) {
            if (this != &other) {
                clear();
                m_type = other.m_type;
                copy_from(other);
            }
            return *this;
        }

        // Type checking
        bool is_null() const { return m_type == null; }
        bool is_boolean() const { return m_type == boolean; }
        bool is_number() const { return m_type == number_integer || m_type == number_float; }
        bool is_string() const { return m_type == string; }
        bool is_array() const { return m_type == array; }
        bool is_object() const { return m_type == object; }

        // Value getters
        bool get_bool() const {
            if (m_type != boolean) throw parse_error("not a boolean");
            return m_value.boolean;
        }

        long long get_int() const {
            if (m_type == number_integer) return m_value.number_integer;
            if (m_type == number_float) return static_cast<long long>(m_value.number_float);
            throw parse_error("not a number");
        }

        double get_float() const {
            if (m_type == number_float) return m_value.number_float;
            if (m_type == number_integer) return static_cast<double>(m_value.number_integer);
            throw parse_error("not a number");
        }

        const std::string& get_string() const {
            if (m_type != string) throw parse_error("not a string");
            return *m_string;
        }

        // Comparison operators
        bool operator==(const json& other) const {
            if (m_type != other.m_type) return false;

            switch (m_type) {
            case null: return true;
            case boolean: return m_value.boolean == other.m_value.boolean;
            case number_integer: return m_value.number_integer == other.m_value.number_integer;
            case number_float: return m_value.number_float == other.m_value.number_float;
            case string: return *m_string == *other.m_string;
            case array: return *m_array == *other.m_array;
            case object: return *m_object == *other.m_object;
            }
            return false;
        }

        bool operator!=(const json& other) const {
            return !(*this == other);
        }

        // Object access operators
        json& operator[](const std::string& key) {
            if (m_type == null) {
                m_type = object;
                m_object = new std::map<std::string, json>();
            }
            if (m_type != object) throw parse_error("not an object");
            return (*m_object)[key];
        }

        const json& operator[](const std::string& key) const {
            if (m_type != object) throw parse_error("not an object");
            std::map<std::string, json>::const_iterator it = m_object->find(key);
            if (it == m_object->end()) throw parse_error("key not found");
            return it->second;
        }

        // Array access operators
        json& operator[](size_t index) {
            if (m_type == null) {
                m_type = array;
                m_array = new std::vector<json>();
            }
            if (m_type != array) throw parse_error("not an array");
            if (index >= m_array->size()) m_array->resize(index + 1);
            return (*m_array)[index];
        }

        const json& operator[](size_t index) const {
            if (m_type != array) throw parse_error("not an array");
            if (index >= m_array->size()) throw parse_error("index out of range");
            return (*m_array)[index];
        }

        // Checked access methods
        json& at(const std::string& key) {
            if (m_type != object) throw parse_error("not an object");
            std::map<std::string, json>::iterator it = m_object->find(key);
            if (it == m_object->end()) throw parse_error("key not found");
            return it->second;
        }

        json& at(size_t index) {
            if (m_type != array) throw parse_error("not an array");
            if (index >= m_array->size()) throw parse_error("index out of range");
            return (*m_array)[index];
        }

        // Object methods
        bool contains(const std::string& key) const {
            if (m_type != object) return false;
            return m_object->find(key) != m_object->end();
        }

        iterator begin() {
            if (m_type != object) throw parse_error("not an object");
            return m_object->begin();
        }

        const_iterator begin() const {
            if (m_type != object) throw parse_error("not an object");
            return m_object->begin();
        }

        iterator end() {
            if (m_type != object) throw parse_error("not an object");
            return m_object->end();
        }

        const_iterator end() const {
            if (m_type != object) throw parse_error("not an object");
            return m_object->end();
        }

        iterator find(const std::string& key) {
            if (m_type != object) throw parse_error("not an object");
            return m_object->find(key);
        }

        const_iterator find(const std::string& key) const {
            if (m_type != object) throw parse_error("not an object");
            return m_object->find(key);
        }

        // Array methods
        void push_back(const json& val) {
            if (m_type == null) {
                m_type = array;
                m_array = new std::vector<json>();
            }
            if (m_type != array) throw parse_error("not an array");
            m_array->push_back(val);
        }

        // Common methods
        size_t size() const {
            if (m_type == array) return m_array->size();
            if (m_type == object) return m_object->size();
            if (m_type == string) return m_string->size();
            return 0;
        }

        bool empty() const {
            return size() == 0;
        }

        // Serialization
        std::string dump(int indent_size = -1) const {
            std::ostringstream ss;
            if (indent_size >= 0) {
                serialize_pretty(ss, 0, indent_size);
            }
            else {
                serialize(ss);
            }
            return ss.str();
        }

        std::string PrettyPrintJson(const std::string& json) {
            std::string result;
            int indent = 0;
            bool in_string = false;

            for (size_t i = 0; i < json.length(); ++i) {
                char c = json[i];

                if (c == '"' && (i == 0 || json[i - 1] != '\\')) {
                    in_string = !in_string;
                }

                if (!in_string) {
                    if (c == '{' || c == '[') {
                        result += c;
                        result += '\n';
                        indent++;
                        result += std::string(indent * 2, ' ');
                    }
                    else if (c == '}' || c == ']') {
                        result += '\n';
                        indent--;
                        result += std::string(indent * 2, ' ');
                        result += c;
                    }
                    else if (c == ',') {
                        result += c;
                        result += '\n';
                        result += std::string(indent * 2, ' ');
                    }
                    else if (c == ':') {
                        result += c;
                        result += ' ';
                    }
                    else {
                        result += c;
                    }
                }
                else {
                    result += c;
                }
            }

            return result;
        }

        // Static parsing methods
        static json parse(const std::string& str) {
            size_t pos = 0;
            return parse_value(str, pos);
        }

        static json parse(std::istream& stream) {
            std::string content((std::istreambuf_iterator<char>(stream)),
                std::istreambuf_iterator<char>());
            size_t pos = 0;
            return parse_value(content, pos);
        }

    private:
        // Member variables
        value_t m_type;
        union {
            bool boolean;
            long long number_integer;
            double number_float;
        } m_value;

        std::string* m_string;
        std::map<std::string, json>* m_object;
        std::vector<json>* m_array;

        // Private helper methods
        void clear() {
            if (m_string) {
                delete m_string;
                m_string = nullptr;
            }
            if (m_object) {
                delete m_object;
                m_object = nullptr;
            }
            if (m_array) {
                delete m_array;
                m_array = nullptr;
            }
        }

        void copy_from(const json& other) {
            m_value = other.m_value;
            if (other.m_string) {
                m_string = new std::string(*other.m_string);
            }
            if (other.m_object) {
                m_object = new std::map<std::string, json>(*other.m_object);
            }
            if (other.m_array) {
                m_array = new std::vector<json>(*other.m_array);
            }
        }

        void serialize(std::ostringstream& ss) const {
            switch (m_type) {
            case null:
                ss << "null";
                break;
            case boolean:
                ss << (m_value.boolean ? "true" : "false");
                break;
            case number_integer:
                ss << m_value.number_integer;
                break;
            case number_float:
                ss << m_value.number_float;
                break;
            case string:
                ss << "\"" << escape_string(*m_string) << "\"";
                break;
            case object:
                ss << "{";
                {
                    bool first = true;
                    for (std::map<std::string, json>::const_iterator it = m_object->begin();
                        it != m_object->end(); ++it) {
                        if (!first) ss << ",";
                        ss << "\"" << escape_string(it->first) << "\":";
                        it->second.serialize(ss);
                        first = false;
                    }
                }
                ss << "}";
                break;
            case array:
                ss << "[";
                for (size_t i = 0; i < m_array->size(); ++i) {
                    if (i > 0) ss << ",";
                    (*m_array)[i].serialize(ss);
                }
                ss << "]";
                break;
            }
        }

        void serialize_pretty(std::ostringstream& ss, int level, int indent_size) const {
            std::string indent(level * indent_size, ' ');
            std::string next_indent((level + 1) * indent_size, ' ');

            switch (m_type) {
            case null:
                ss << "null";
                break;
            case boolean:
                ss << (m_value.boolean ? "true" : "false");
                break;
            case number_integer:
                ss << m_value.number_integer;
                break;
            case number_float:
                ss << m_value.number_float;
                break;
            case string:
                ss << "\"" << escape_string(*m_string) << "\"";
                break;
            case object:
                if (!m_object || m_object->empty()) {
                    ss << "{}";
                }
                else {
                    ss << "{";
                    bool first = true;
                    for (std::map<std::string, json>::const_iterator it = m_object->begin();
                        it != m_object->end(); ++it) {
                        if (!first) ss << ",";
                        ss << "\n" << next_indent << "\"" << escape_string(it->first) << "\": ";
                        it->second.serialize_pretty(ss, level + 1, indent_size);
                        first = false;
                    }
                    ss << "\n" << indent << "}";
                }
                break;
            case array:
                if (!m_array || m_array->empty()) {
                    ss << "[]";
                }
                else {
                    ss << "[";
                    for (size_t i = 0; i < m_array->size(); ++i) {
                        if (i > 0) ss << ", ";
                        (*m_array)[i].serialize(ss);
                    }
                    ss << "]";
                }
                break;
            }
        }

        // Static helper methods
        static std::string escape_string(const std::string& str) {
            std::string result;
            for (size_t i = 0; i < str.length(); ++i) {
                char c = str[i];
                switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
                }
            }
            return result;
        }

        static void skip_whitespace(const std::string& str, size_t& pos) {
            while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\t' ||
                str[pos] == '\n' || str[pos] == '\r')) {
                ++pos;
            }
        }

        static json parse_value(const std::string& str, size_t& pos) {
            skip_whitespace(str, pos);
            if (pos >= str.length()) throw parse_error("unexpected end of input");

            char c = str[pos];
            if (c == 'n') return parse_null(str, pos);
            if (c == 't' || c == 'f') return parse_boolean(str, pos);
            if (c == '"') return parse_string(str, pos);
            if (c == '[') return parse_array(str, pos);
            if (c == '{') return parse_object(str, pos);
            if (c == '-' || (c >= '0' && c <= '9')) return parse_number(str, pos);

            throw parse_error("unexpected character");
        }

        static json parse_null(const std::string& str, size_t& pos) {
            if (str.substr(pos, 4) == "null") {
                pos += 4;
                return json();
            }
            throw parse_error("invalid null value");
        }

        static json parse_boolean(const std::string& str, size_t& pos) {
            if (str.substr(pos, 4) == "true") {
                pos += 4;
                return json(true);
            }
            if (str.substr(pos, 5) == "false") {
                pos += 5;
                return json(false);
            }
            throw parse_error("invalid boolean value");
        }

        static json parse_string(const std::string& str, size_t& pos) {
            if (str[pos] != '"') throw parse_error("expected '\"'");
            ++pos;

            std::string result;
            while (pos < str.length() && str[pos] != '"') {
                if (str[pos] == '\\') {
                    ++pos;
                    if (pos >= str.length()) throw parse_error("unexpected end in string");
                    char escaped = str[pos];
                    switch (escaped) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: throw parse_error("invalid escape sequence");
                    }
                }
                else {
                    result += str[pos];
                }
                ++pos;
            }

            if (pos >= str.length()) throw parse_error("unterminated string");
            ++pos;
            return json(result);
        }

        static json parse_number(const std::string& str, size_t& pos) {
            size_t start = pos;
            bool is_float = false;

            if (str[pos] == '-') ++pos;

            if (pos >= str.length() || str[pos] < '0' || str[pos] > '9') {
                throw parse_error("invalid number");
            }

            while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9') ++pos;

            if (pos < str.length() && str[pos] == '.') {
                is_float = true;
                ++pos;
                while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9') ++pos;
            }

            if (pos < str.length() && (str[pos] == 'e' || str[pos] == 'E')) {
                is_float = true;
                ++pos;
                if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) ++pos;
                while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9') ++pos;
            }

            std::string num_str = str.substr(start, pos - start);
            if (is_float) {
                return json(atof(num_str.c_str()));
            }
            else {
                long long result = 0;
                bool negative = false;
                size_t i = 0;
                if (num_str[0] == '-') {
                    negative = true;
                    i = 1;
                }
                for (; i < num_str.length(); ++i) {
                    result = result * 10 + (num_str[i] - '0');
                }
                return json(negative ? -result : result);
            }
        }

        static json parse_array(const std::string& str, size_t& pos) {
            if (str[pos] != '[') throw parse_error("expected '['");
            ++pos;

            json result;
            result.m_type = array;
            result.m_array = new std::vector<json>();

            skip_whitespace(str, pos);
            if (pos < str.length() && str[pos] == ']') {
                ++pos;
                return result;
            }

            while (true) {
                result.m_array->push_back(parse_value(str, pos));
                skip_whitespace(str, pos);

                if (pos >= str.length()) throw parse_error("unterminated array");

                if (str[pos] == ']') {
                    ++pos;
                    break;
                }
                else if (str[pos] == ',') {
                    ++pos;
                    skip_whitespace(str, pos);
                }
                else {
                    throw parse_error("expected ',' or ']'");
                }
            }

            return result;
        }

        static json parse_object(const std::string& str, size_t& pos) {
            if (str[pos] != '{') throw parse_error("expected '{'");
            ++pos;

            json result;
            result.m_type = object;
            result.m_object = new std::map<std::string, json>();

            skip_whitespace(str, pos);
            if (pos < str.length() && str[pos] == '}') {
                ++pos;
                return result;
            }

            while (true) {
                skip_whitespace(str, pos);
                json key = parse_string(str, pos);
                skip_whitespace(str, pos);

                if (pos >= str.length() || str[pos] != ':') {
                    throw parse_error("expected ':'");
                }
                ++pos;

                json value = parse_value(str, pos);
                (*result.m_object)[key.get_string()] = value;

                skip_whitespace(str, pos);
                if (pos >= str.length()) throw parse_error("unterminated object");

                if (str[pos] == '}') {
                    ++pos;
                    break;
                }
                else if (str[pos] == ',') {
                    ++pos;
                }
                else {
                    throw parse_error("expected ',' or '}'");
                }
            }

            return result;
        }
    };

    // Template helper functions
    template <typename T>
    T JsonGet(nlohmann::json& value, const std::string& key, T defval = T()) {
        if (key.empty() || key == "" || value.is_null()) return defval;
        if (value.is_object()) {
            nlohmann::json::iterator it = value.find(key);
            if (it != value.end()) {
                try {
                    nlohmann::json& item = it->second;
                    return defval;
                }
                catch (...) {
                    return defval;
                }
            }
        }
        return defval;
    }

    template <>
    inline std::string JsonGet<std::string>(nlohmann::json& value, const std::string& key, std::string defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        nlohmann::json::iterator it = value.find(key);
        if (it != value.end()) {
            try {
                if (it->second.is_string()) {
                    return it->second.get_string();
                }
            }
            catch (...) {}
        }
        return defval;
    }

    template <>
    inline int JsonGet<int>(nlohmann::json& value, const std::string& key, int defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        nlohmann::json::iterator it = value.find(key);
        if (it != value.end()) {
            try {
                if (it->second.is_number()) {
                    return static_cast<int>(it->second.get_int());
                }
            }
            catch (...) {}
        }
        return defval;
    }

    template <>
    inline long long JsonGet<long long>(nlohmann::json& value, const std::string& key, long long defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        nlohmann::json::iterator it = value.find(key);
        if (it != value.end()) {
            try {
                if (it->second.is_number()) {
                    return it->second.get_int();
                }
            }
            catch (...) {}
        }
        return defval;
    }

    template <>
    inline double JsonGet<double>(nlohmann::json& value, const std::string& key, double defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        nlohmann::json::iterator it = value.find(key);
        if (it != value.end()) {
            try {
                if (it->second.is_number()) {
                    return it->second.get_float();
                }
            }
            catch (...) {}
        }
        return defval;
    }

    template <>
    inline bool JsonGet<bool>(nlohmann::json& value, const std::string& key, bool defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        nlohmann::json::iterator it = value.find(key);
        if (it != value.end()) {
            try {
                if (it->second.is_boolean()) {
                    return it->second.get_bool();
                }
            }
            catch (...) {}
        }
        return defval;
    }
}
