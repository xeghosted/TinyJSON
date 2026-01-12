#pragma once

namespace tinyjson {
    class json;
}

namespace tinyjson {
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

        // Custom iterator for ordered object
        class iterator {
        private:
            std::vector<std::pair<std::string, json>>* m_data;
            size_t m_index;
        public:
            iterator(std::vector<std::pair<std::string, json>>* data, size_t index)
                : m_data(data), m_index(index) {}

            std::pair<std::string, json>& operator*() {
                return (*m_data)[m_index];
            }

            std::pair<std::string, json>* operator->() {
                return &(*m_data)[m_index];
            }

            iterator& operator++() {
                ++m_index;
                return *this;
            }

            bool operator==(const iterator& other) const {
                return m_data == other.m_data && m_index == other.m_index;
            }

            bool operator!=(const iterator& other) const {
                return !(*this == other);
            }
        };

        class const_iterator {
        private:
            const std::vector<std::pair<std::string, json>>* m_data;
            size_t m_index;
        public:
            const_iterator(const std::vector<std::pair<std::string, json>>* data, size_t index)
                : m_data(data), m_index(index) {}

            const std::pair<std::string, json>& operator*() const {
                return (*m_data)[m_index];
            }

            const std::pair<std::string, json>* operator->() const {
                return &(*m_data)[m_index];
            }

            const_iterator& operator++() {
                ++m_index;
                return *this;
            }

            bool operator==(const const_iterator& other) const {
                return m_data == other.m_data && m_index == other.m_index;
            }

            bool operator!=(const const_iterator& other) const {
                return !(*this == other);
            }
        };

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
            case object: {
                if (m_object->size() != other.m_object->size()) return false;
                for (size_t i = 0; i < m_object->size(); ++i) {
                    if ((*m_object)[i].first != (*other.m_object)[i].first ||
                        (*m_object)[i].second != (*other.m_object)[i].second) {
                        return false;
                    }
                }
                return true;
            }
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
                m_object = new std::vector<std::pair<std::string, json>>();
            }
            if (m_type != object) throw parse_error("not an object");

            // Search for existing key
            for (size_t i = 0; i < m_object->size(); ++i) {
                if ((*m_object)[i].first == key) {
                    return (*m_object)[i].second;
                }
            }

            // Key not found, add new entry
            m_object->push_back(std::make_pair(key, json()));
            return m_object->back().second;
        }

        const json& operator[](const std::string& key) const {
            if (m_type != object) throw parse_error("not an object");

            for (size_t i = 0; i < m_object->size(); ++i) {
                if ((*m_object)[i].first == key) {
                    return (*m_object)[i].second;
                }
            }
            throw parse_error("key not found");
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

            for (size_t i = 0; i < m_object->size(); ++i) {
                if ((*m_object)[i].first == key) {
                    return (*m_object)[i].second;
                }
            }
            throw parse_error("key not found");
        }

        json& at(size_t index) {
            if (m_type != array) throw parse_error("not an array");
            if (index >= m_array->size()) throw parse_error("index out of range");
            return (*m_array)[index];
        }

        // Object methods
        bool contains(const std::string& key) const {
            if (m_type != object) return false;
            for (size_t i = 0; i < m_object->size(); ++i) {
                if ((*m_object)[i].first == key) {
                    return true;
                }
            }
            return false;
        }

        // Remove a key from object (returns true if key was found and removed)
        bool erase(const std::string& key) {
            if (m_type != object) return false;
            for (size_t i = 0; i < m_object->size(); ++i) {
                if ((*m_object)[i].first == key) {
                    m_object->erase(m_object->begin() + i);
                    return true;
                }
            }
            return false;
        }

        iterator begin() {
            if (m_type != object) throw parse_error("not an object");
            return iterator(m_object, 0);
        }

        const_iterator begin() const {
            if (m_type != object) throw parse_error("not an object");
            return const_iterator(m_object, 0);
        }

        iterator end() {
            if (m_type != object) throw parse_error("not an object");
            return iterator(m_object, m_object->size());
        }

        const_iterator end() const {
            if (m_type != object) throw parse_error("not an object");
            return const_iterator(m_object, m_object->size());
        }

        iterator find(const std::string& key) {
            if (m_type != object) throw parse_error("not an object");
            for (size_t i = 0; i < m_object->size(); ++i) {
                if ((*m_object)[i].first == key) {
                    return iterator(m_object, i);
                }
            }
            return end();
        }

        const_iterator find(const std::string& key) const {
            if (m_type != object) throw parse_error("not an object");
            for (size_t i = 0; i < m_object->size(); ++i) {
                if ((*m_object)[i].first == key) {
                    return const_iterator(m_object, i);
                }
            }
            return end();
        }

        // Array methods
        void push_back(const json& value) {
            if (m_type == null) {
                m_type = array;
                m_array = new std::vector<json>();
            }
            if (m_type != array) throw parse_error("not an array");
            m_array->push_back(value);
        }

        size_t size() const {
            if (m_type == array) return m_array->size();
            if (m_type == object) return m_object->size();
            if (m_type == string) return m_string->size();
            return 0;
        }

        bool empty() const {
            if (m_type == array) return m_array->empty();
            if (m_type == object) return m_object->empty();
            if (m_type == string) return m_string->empty();
            return true;
        }

        // Get value with default - safe access with type checking
        template<typename T>
        T value(const std::string& key, const T& default_val) const {
            if (m_type != object) return default_val;

            for (size_t i = 0; i < m_object->size(); ++i) {
                if ((*m_object)[i].first == key) {
                    return get_value_helper<T>((*m_object)[i].second, default_val);
                }
            }
            return default_val;
        }

        // Path-based access (e.g., "user.settings.theme" or "options.0.enabled")
        json& at_path(const std::string& path) {
            std::vector<std::string> parts = split_path(path);
            json* current = this;

            for (size_t i = 0; i < parts.size(); ++i) {
                // Check if this part is a number (array index)
                bool is_index = is_numeric(parts[i]);

                if (is_index) {
                    if (current->m_type != array) {
                        throw parse_error("path element is not an array");
                    }
                    size_t index = string_to_size_t(parts[i]);
                    if (index >= current->m_array->size()) {
                        throw parse_error("array index out of range");
                    }
                    current = &(*current->m_array)[index];
                }
                else {
                    if (current->m_type != object) {
                        throw parse_error("path element is not an object");
                    }

                    bool found = false;
                    for (size_t j = 0; j < current->m_object->size(); ++j) {
                        if ((*current->m_object)[j].first == parts[i]) {
                            current = &(*current->m_object)[j].second;
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        throw parse_error("path not found: " + path);
                    }
                }
            }

            return *current;
        }

        const json& at_path(const std::string& path) const {
            std::vector<std::string> parts = split_path(path);
            const json* current = this;

            for (size_t i = 0; i < parts.size(); ++i) {
                bool is_index = is_numeric(parts[i]);

                if (is_index) {
                    if (current->m_type != array) {
                        throw parse_error("path element is not an array");
                    }
                    size_t index = string_to_size_t(parts[i]);
                    if (index >= current->m_array->size()) {
                        throw parse_error("array index out of range");
                    }
                    current = &(*current->m_array)[index];
                }
                else {
                    if (current->m_type != object) {
                        throw parse_error("path element is not an object");
                    }

                    bool found = false;
                    for (size_t j = 0; j < current->m_object->size(); ++j) {
                        if ((*current->m_object)[j].first == parts[i]) {
                            current = &(*current->m_object)[j].second;
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        throw parse_error("path not found: " + path);
                    }
                }
            }

            return *current;
        }

        bool has_path(const std::string& path) const {
            try {
                at_path(path);
                return true;
            }
            catch (...) {
                return false;
            }
        }

        void set_path(const std::string& path, const json& value) {
            std::vector<std::string> parts = split_path(path);
            if (parts.empty()) return;

            json* current = this;

            // Navigate/create path to second-to-last element
            for (size_t i = 0; i < parts.size() - 1; ++i) {
                bool is_index = is_numeric(parts[i]);

                if (is_index) {
                    if (current->m_type != array) {
                        throw parse_error("path element is not an array");
                    }
                    size_t index = string_to_size_t(parts[i]);
                    if (index >= current->m_array->size()) {
                        throw parse_error("array index out of range");
                    }
                    current = &(*current->m_array)[index];
                }
                else {
                    if (current->m_type == null) {
                        current->m_type = object;
                        current->m_object = new std::vector<std::pair<std::string, json>>();
                    }

                    if (current->m_type != object) {
                        throw parse_error("path element is not an object");
                    }

                    bool found = false;
                    for (size_t j = 0; j < current->m_object->size(); ++j) {
                        if ((*current->m_object)[j].first == parts[i]) {
                            current = &(*current->m_object)[j].second;
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        current->m_object->push_back(std::make_pair(parts[i], json()));
                        current = &current->m_object->back().second;
                    }
                }
            }

            // Set the final value
            bool last_is_index = is_numeric(parts.back());

            if (last_is_index) {
                if (current->m_type != array) {
                    throw parse_error("path element is not an array");
                }
                size_t index = string_to_size_t(parts.back());
                if (index >= current->m_array->size()) {
                    throw parse_error("array index out of range");
                }
                (*current->m_array)[index] = value;
            }
            else {
                if (current->m_type == null) {
                    current->m_type = object;
                    current->m_object = new std::vector<std::pair<std::string, json>>();
                }

                if (current->m_type != object) {
                    throw parse_error("path element is not an object");
                }

                (*current)[parts.back()] = value;
            }
        }

        // Path-based value with default
        template<typename T>
        T value_at_path(const std::string& path, const T& default_val) const {
            try {
                const json& val = at_path(path);
                return get_value_helper<T>(val, default_val);
            }
            catch (...) {
                return default_val;
            }
        }

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
            m_type = null;
            m_value.number_integer = 0;
        }

        // Serialization
        std::string dump(int indent = -1, int current_indent = 0) const {
            std::string result;

            switch (m_type) {
            case null:
                result = "null";
                break;
            case boolean:
                result = m_value.boolean ? "true" : "false";
                break;
            case number_integer:
                result = int_to_string(m_value.number_integer);
                break;
            case number_float: {
                char buffer[64];
                sprintf(buffer, "%.17g", m_value.number_float);
                result = buffer;
                break;
            }
            case string:
                result = "\"" + escape_string(*m_string) + "\"";
                break;
            case array: {
                result = "[";
                if (indent >= 0 && !m_array->empty()) {
                    result += "\n";
                }
                for (size_t i = 0; i < m_array->size(); ++i) {
                    if (indent >= 0) {
                        result += std::string((current_indent + indent), ' ');
                    }
                    result += (*m_array)[i].dump(indent, current_indent + indent);
                    if (i < m_array->size() - 1) {
                        result += ",";
                    }
                    if (indent >= 0) {
                        result += "\n";
                    }
                }
                if (indent >= 0 && !m_array->empty()) {
                    result += std::string(current_indent, ' ');
                }
                result += "]";
                break;
            }
            case object: {
                result = "{";
                if (indent >= 0 && !m_object->empty()) {
                    result += "\n";
                }
                for (size_t i = 0; i < m_object->size(); ++i) {
                    if (indent >= 0) {
                        result += std::string((current_indent + indent), ' ');
                    }
                    result += "\"" + escape_string((*m_object)[i].first) + "\":";
                    if (indent >= 0) {
                        result += " ";
                    }
                    result += (*m_object)[i].second.dump(indent, current_indent + indent);
                    if (i < m_object->size() - 1) {
                        result += ",";
                    }
                    if (indent >= 0) {
                        result += "\n";
                    }
                }
                if (indent >= 0 && !m_object->empty()) {
                    result += std::string(current_indent, ' ');
                }
                result += "}";
                break;
            }
            }

            return result;
        }

        // Parsing
        static json parse(const std::string& str) {
            size_t pos = 0;
            skip_whitespace(str, pos);
            if (pos >= str.length()) throw parse_error("empty input");
            json result = parse_value(str, pos);
            skip_whitespace(str, pos);
            if (pos < str.length()) throw parse_error("unexpected data after JSON");
            return result;
        }

        // File I/O operations
        static json load_from_file(const std::string& filepath) {
            FILE* file = fopen(filepath.c_str(), "rb");
            if (!file) {
                throw parse_error("could not open file: " + filepath);
            }

            // Get file size
            fseek(file, 0, SEEK_END);
            long size = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (size <= 0) {
                fclose(file);
                throw parse_error("empty or invalid file: " + filepath);
            }

            // Read file contents
            std::string content;
            content.resize(size);
            size_t read = fread(&content[0], 1, size, file);
            fclose(file);

            if (read != static_cast<size_t>(size)) {
                throw parse_error("failed to read file: " + filepath);
            }

            return parse(content);
        }

        bool save_to_file(const std::string& filepath, int indent = 2) const {
            FILE* file = fopen(filepath.c_str(), "wb");
            if (!file) {
                return false;
            }

            std::string content = dump(indent);
            size_t written = fwrite(content.c_str(), 1, content.length(), file);
            fclose(file);

            return written == content.length();
        }

        // Save to file with error message
        bool save_to_file_verbose(const std::string& filepath, int indent, std::string& error_msg) const {
            FILE* file = fopen(filepath.c_str(), "wb");
            if (!file) {
                error_msg = "Failed to open file for writing: " + filepath;
                return false;
            }

            std::string content = dump(indent);
            size_t written = fwrite(content.c_str(), 1, content.length(), file);
            int flush_result = fflush(file);
            fclose(file);

            if (written != content.length()) {
                error_msg = "Failed to write complete data. Wrote " + size_to_string(written) +
                    " of " + size_to_string(content.length()) + " bytes";
                return false;
            }

            if (flush_result != 0) {
                error_msg = "Failed to flush file buffer";
                return false;
            }

            return true;
        }

        // Load from file with error message
        static json load_from_file_verbose(const std::string& filepath, std::string& error_msg) {
            FILE* file = fopen(filepath.c_str(), "rb");
            if (!file) {
                error_msg = "Could not open file: " + filepath;
                throw parse_error(error_msg);
            }

            // Get file size
            fseek(file, 0, SEEK_END);
            long size = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (size <= 0) {
                fclose(file);
                error_msg = "Empty or invalid file: " + filepath;
                throw parse_error(error_msg);
            }

            // Read file contents
            std::string content;
            content.resize(size);
            size_t read = fread(&content[0], 1, size, file);
            fclose(file);

            if (read != static_cast<size_t>(size)) {
                error_msg = "Failed to read file completely. Read " + size_to_string(read) +
                    " of " + size_to_string(static_cast<size_t>(size)) + " bytes";
                throw parse_error(error_msg);
            }

            try {
                return parse(content);
            }
            catch (const parse_error& e) {
                error_msg = "Failed to parse JSON: " + std::string(e.what());
                throw;
            }
        }

    private:
        value_t m_type;
        union {
            bool boolean;
            long long number_integer;
            double number_float;
        } m_value;
        std::string* m_string;
        std::vector<std::pair<std::string, json>>* m_object;
        std::vector<json>* m_array;

        void copy_from(const json& other) {
            m_value = other.m_value;
            if (other.m_string) {
                m_string = new std::string(*other.m_string);
            }
            if (other.m_object) {
                m_object = new std::vector<std::pair<std::string, json>>(*other.m_object);
            }
            if (other.m_array) {
                m_array = new std::vector<json>(*other.m_array);
            }
        }

        // Helper for value() method with type checking
        template<typename T>
        static T get_value_helper(const json& j, const T& default_val) {
            return default_val;  // Generic fallback
        }

        // Helper to split path by dots
        static std::vector<std::string> split_path(const std::string& path) {
            std::vector<std::string> parts;
            std::string current;

            for (size_t i = 0; i < path.length(); ++i) {
                if (path[i] == '.') {
                    if (!current.empty()) {
                        parts.push_back(current);
                        current.clear();
                    }
                }
                else {
                    current += path[i];
                }
            }

            if (!current.empty()) {
                parts.push_back(current);
            }

            return parts;
        }

        // Xbox 360 compatible number to string conversion
        static std::string int_to_string(long long value) {
            char buffer[32];
            sprintf(buffer, "%lld", value);
            return std::string(buffer);
        }

        static std::string size_to_string(size_t value) {
            char buffer[32];
            sprintf(buffer, "%zu", static_cast<unsigned long>(value));
            return std::string(buffer);
        }

        // Check if string is numeric (for array indices)
        static bool is_numeric(const std::string& str) {
            if (str.empty()) return false;
            for (size_t i = 0; i < str.length(); ++i) {
                if (str[i] < '0' || str[i] > '9') return false;
            }
            return true;
        }

        // Convert string to size_t
        static size_t string_to_size_t(const std::string& str) {
            size_t result = 0;
            for (size_t i = 0; i < str.length(); ++i) {
                result = result * 10 + (str[i] - '0');
            }
            return result;
        }

        static std::string escape_string(const std::string& str) {
            std::string result;
            for (size_t i = 0; i < str.length(); ++i) {
                switch (str[i]) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (str[i] < 0x20) {
                        char buffer[7];
                        sprintf(buffer, "\\u%04x", static_cast<unsigned char>(str[i]));
                        result += buffer;
                    }
                    else {
                        result += str[i];
                    }
                }
            }
            return result;
        }

        static void skip_whitespace(const std::string& str, size_t& pos) {
            while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\n' ||
                str[pos] == '\r' || str[pos] == '\t')) {
                ++pos;
            }
        }

        static json parse_value(const std::string& str, size_t& pos) {
            skip_whitespace(str, pos);
            if (pos >= str.length()) throw parse_error("unexpected end of input");

            if (str[pos] == 'n') return parse_null(str, pos);
            if (str[pos] == 't' || str[pos] == 'f') return parse_boolean(str, pos);
            if (str[pos] == '"') return parse_string(str, pos);
            if (str[pos] == '[') return parse_array(str, pos);
            if (str[pos] == '{') return parse_object(str, pos);
            if (str[pos] == '-' || (str[pos] >= '0' && str[pos] <= '9')) {
                return parse_number(str, pos);
            }

            throw parse_error("unexpected character");
        }

        static json parse_null(const std::string& str, size_t& pos) {
            if (str.substr(pos, 4) != "null") throw parse_error("expected 'null'");
            pos += 4;
            return json();
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
            throw parse_error("expected 'true' or 'false'");
        }

        static json parse_string(const std::string& str, size_t& pos) {
            if (str[pos] != '"') throw parse_error("expected '\"'");
            ++pos;

            std::string result;
            while (pos < str.length() && str[pos] != '"') {
                if (str[pos] == '\\') {
                    ++pos;
                    if (pos >= str.length()) throw parse_error("unterminated string");

                    switch (str[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        ++pos;
                        if (pos + 3 >= str.length()) throw parse_error("invalid unicode escape");
                        unsigned int codepoint = 0;
                        for (int i = 0; i < 4; ++i) {
                            char c = str[pos + i];
                            codepoint <<= 4;
                            if (c >= '0' && c <= '9') codepoint |= (c - '0');
                            else if (c >= 'a' && c <= 'f') codepoint |= (c - 'a' + 10);
                            else if (c >= 'A' && c <= 'F') codepoint |= (c - 'A' + 10);
                            else throw parse_error("invalid unicode escape");
                        }
                        if (codepoint <= 0x7F) {
                            result += static_cast<char>(codepoint);
                        }
                        else if (codepoint <= 0x7FF) {
                            result += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
                            result += static_cast<char>(0x80 | (codepoint & 0x3F));
                        }
                        else {
                            result += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
                            result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                            result += static_cast<char>(0x80 | (codepoint & 0x3F));
                        }
                        pos += 3;
                        break;
                    }
                    default:
                        throw parse_error("invalid escape sequence");
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
            result.m_object = new std::vector<std::pair<std::string, json>>();

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
                result.m_object->push_back(std::make_pair(key.get_string(), value));

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
    T JsonGet(tinyjson::json& value, const std::string& key, T defval = T()) {
        if (key.empty() || key == "" || value.is_null()) return defval;
        if (value.is_object()) {
            tinyjson::json::iterator it = value.find(key);
            if (it != value.end()) {
                try {
                    tinyjson::json& item = it->second;
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
    inline std::string JsonGet<std::string>(tinyjson::json& value, const std::string& key, std::string defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        tinyjson::json::iterator it = value.find(key);
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
    inline int JsonGet<int>(tinyjson::json& value, const std::string& key, int defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        tinyjson::json::iterator it = value.find(key);
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
    inline long long JsonGet<long long>(tinyjson::json& value, const std::string& key, long long defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        tinyjson::json::iterator it = value.find(key);
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
    inline double JsonGet<double>(tinyjson::json& value, const std::string& key, double defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        tinyjson::json::iterator it = value.find(key);
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
    inline bool JsonGet<bool>(tinyjson::json& value, const std::string& key, bool defval) {
        if (key.empty() || value.is_null() || !value.is_object()) return defval;
        tinyjson::json::iterator it = value.find(key);
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

    // Template specializations for get_value_helper
    template <>
    inline std::string json::get_value_helper<std::string>(const json& j, const std::string& default_val) {
        if (j.is_string()) {
            return j.get_string();
        }
        return default_val;
    }

    template <>
    inline int json::get_value_helper<int>(const json& j, const int& default_val) {
        if (j.is_number()) {
            return static_cast<int>(j.get_int());
        }
        return default_val;
    }

    template <>
    inline long long json::get_value_helper<long long>(const json& j, const long long& default_val) {
        if (j.is_number()) {
            return j.get_int();
        }
        return default_val;
    }

    template <>
    inline double json::get_value_helper<double>(const json& j, const double& default_val) {
        if (j.is_number()) {
            return j.get_float();
        }
        return default_val;
    }

    template <>
    inline float json::get_value_helper<float>(const json& j, const float& default_val) {
        if (j.is_number()) {
            return static_cast<float>(j.get_float());
        }
        return default_val;
    }

    template <>
    inline bool json::get_value_helper<bool>(const json& j, const bool& default_val) {
        if (j.is_boolean()) {
            return j.get_bool();
        }
        return default_val;
    }

    template <>
    inline unsigned int json::get_value_helper<unsigned int>(const json& j, const unsigned int& default_val) {
        if (j.is_number()) {
            return static_cast<unsigned int>(j.get_int());
        }
        return default_val;
    }
}