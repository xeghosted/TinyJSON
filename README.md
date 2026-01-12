# TinyJSON

A lightweight, header-only JSON library optimized for Xbox 360 and legacy C++ environments. Features insertion-order preservation, path-based access, and zero C++11 dependencies.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++98](https://img.shields.io/badge/C%2B%2B-98-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![Platform](https://img.shields.io/badge/Platform-Xbox%20360-green.svg)](https://en.wikipedia.org/wiki/Xbox_360)

## ✨ Features

- 🎯 **Insertion Order Preservation** - Keys maintain the order they were added (not alphabetically sorted)
- 🛣️ **Path-Based Access** - Access nested values with dot notation: `"player.stats.health"`
- 🛡️ **Safe Access with Defaults** - Never crash on missing keys with `.value()` methods
- 💾 **File I/O Built-in** - Load and save JSON files with one function call
- 🎮 **Xbox 360 Compatible** - No C++11 features, works with older compilers
- 📦 **Header-Only** - Just include `Json.h` and you're ready to go
- 🗑️ **Key Removal** - Dynamically add and remove object keys
- 🔍 **Array Index Paths** - Access array elements via paths: `"options.0.label"`

## 📋 Table of Contents

- [Quick Start](#quick-start)
- [Installation](#installation)
- [Basic Usage](#basic-usage)
- [Path-Based Access](#path-based-access)
- [Safe Access with Defaults](#safe-access-with-defaults)
- [File I/O](#file-io)
- [Key Removal](#key-removal)
- [Xbox 360 Compatibility](#xbox-360-compatibility)
- [API Reference](#api-reference)
- [Examples](#examples)
- [Building](#building)
- [License](#license)

## 🚀 Quick Start

```cpp
#include "Json.h"

// Create JSON object
tinyjson::json config;
config["username"] = "Player1";
config["level"] = 42;
config["settings"]["volume"] = 75;

// Access with paths
int volume = config.value_at_path<int>("settings.volume", 50);

// Save to file
config.save_to_file("game:\\config.json", 2);

// Load from file
tinyjson::json loaded = tinyjson::json::load_from_file("game:\\config.json");
```

## 📦 Installation

### Header-Only Library

Simply copy `Json.h` to your project and include it:

```cpp
#include "Json.h"
```

### Requirements

- C++98 or later
- Standard library: `<string>`, `<vector>`, `<map>`, `<exception>`, `<cstdio>`
- No external dependencies

### Xbox 360 Setup

```cpp
// Works directly with Xbox 360 SDK
#include "Json.h"

// Use Xbox 360 file paths
tinyjson::json config;
config.save_to_file("game:\\settings.json", 2);
```

## 📖 Basic Usage

### Creating JSON Objects

```cpp
tinyjson::json obj;

// Basic types
obj["name"] = "Dominik";
obj["age"] = 25;
obj["score"] = 12345.67;
obj["active"] = true;
obj["data"] = nullptr;

// Nested objects
obj["settings"]["theme"] = "dark";
obj["settings"]["language"] = "en";

// Arrays
obj["items"].push_back("sword");
obj["items"].push_back("shield");
obj["items"].push_back("potion");
```

### Parsing JSON

```cpp
std::string json_str = R"({
    "player": {
        "name": "Hero",
        "health": 100
    }
})";

tinyjson::json parsed = tinyjson::json::parse(json_str);
std::string name = parsed["player"]["name"].get_string();
int health = parsed["player"]["health"].get_int();
```

### Serialization

```cpp
tinyjson::json obj;
obj["key"] = "value";

// Pretty print with indentation
std::string pretty = obj.dump(2);

// Compact (no whitespace)
std::string compact = obj.dump(-1);
```

## 🛣️ Path-Based Access

Access nested values using dot notation:

```cpp
tinyjson::json game;
game["player"]["stats"]["health"] = 100;
game["player"]["stats"]["mana"] = 50;
game["player"]["inventory"]["weapon"] = "Sword";

// Access with paths
int health = game.at_path("player.stats.health").get_int();
std::string weapon = game.at_path("player.inventory.weapon").get_string();

// Check if path exists
if (game.has_path("player.stats.stamina")) {
    // Path exists
}

// Set values via path (creates intermediate objects)
game.set_path("player.stats.stamina", tinyjson::json(75));
game.set_path("settings.graphics.quality", tinyjson::json("Ultra"));
```

### Array Index Paths

```cpp
tinyjson::json menu;
menu["options"][0]["label"] = "God Mode";
menu["options"][0]["enabled"] = false;
menu["options"][1]["label"] = "Infinite Ammo";
menu["options"][1]["enabled"] = true;

// Access array elements via paths
std::string label = menu.at_path("options.0.label").get_string();
bool enabled = menu.at_path("options.1.enabled").get_bool();

// Modify array elements
menu.set_path("options.0.enabled", tinyjson::json(true));
```

## 🛡️ Safe Access with Defaults

Never crash on missing keys:

```cpp
tinyjson::json config;
config["graphics"]["quality"] = "High";
config["audio"]["volume"] = 75;

// Safe access - returns default if key doesn't exist
int fps = config.value<int>("fps", 60);                    // Missing key → 60
std::string quality = config["graphics"].value<std::string>("quality", "Low"); // "High"
bool vsync = config["graphics"].value<bool>("vsync", true); // Missing → true

// Path-based safe access
int lives = config.value_at_path<int>("game.lives", 3);
std::string weapon = config.value_at_path<std::string>("player.weapon", "Default");
```

**Supported Types:**
- `int`, `long long`, `unsigned int`
- `float`, `double`
- `bool`
- `std::string`

## 💾 File I/O

### Simple File Operations

```cpp
tinyjson::json config;
config["setting1"] = "value1";
config["setting2"] = 42;

// Save to file
bool success = config.save_to_file("config.json", 2);

// Load from file
tinyjson::json loaded = tinyjson::json::load_from_file("config.json");
```

### Verbose File Operations (with error messages)

```cpp
std::string error_msg;

// Save with error details
if (!config.save_to_file_verbose("config.json", 2, error_msg)) {
    std::cout << "Save failed: " << error_msg << std::endl;
}

// Load with error details
try {
    tinyjson::json loaded = tinyjson::json::load_from_file_verbose("config.json", error_msg);
} catch (const tinyjson::parse_error& e) {
    std::cout << "Load failed: " << error_msg << std::endl;
}
```

### Xbox 360 File Paths

```cpp
// Try different path formats
std::vector<std::string> paths;
paths.push_back("game:\\config.json");      // Game partition (usually works)
paths.push_back("hdd:\\config.json");       // Hard drive
paths.push_back("D:\\config.json");         // Alternative

for (size_t i = 0; i < paths.size(); i++) {
    if (config.save_to_file(paths[i], 2)) {
        // Success!
        break;
    }
}
```

## 🗑️ Key Removal

```cpp
tinyjson::json user;
user["username"] = "player1";
user["password"] = "secret123";
user["email"] = "player@example.com";

// Remove sensitive data
bool removed = user.erase("password");  // Returns true if key existed

// Remove multiple keys
user.erase("session_token");
user.erase("api_key");
```

## 🎮 Xbox 360 Compatibility

TinyJSON is designed to work with Xbox 360's older C++ compiler (similar to Visual C++ 2010).

### ❌ Avoid These (C++11 Features)

```cpp
// DON'T USE - Won't compile on Xbox 360
std::vector<std::string> items = {"item1", "item2", "item3"};  // Initializer lists
for (const auto& item : items) { }                              // Range-based for
std::string s = std::to_string(123);                            // std::to_string
```

### ✅ Use These Instead

```cpp
// DO THIS - Xbox 360 compatible
std::vector<std::string> items;
items.push_back("item1");
items.push_back("item2");
items.push_back("item3");

// Iteration method 1: Traditional for loop
for (size_t i = 0; i < items.size(); i++) {
    std::string item = items[i];
}

// Iteration method 2: for each (Microsoft extension)
for each (const std::string& item in items) {
    // use item
}

// Number to string conversion
char buffer[64];
sprintf(buffer, "%d", 123);
std::string s = buffer;
```

### Building Menu Systems (Xbox 360 Style)

```cpp
tinyjson::json menu;
menu["title"] = "xbNetwork Menu";

// Add options individually
tinyjson::json opt1;
opt1["id"] = "god_mode";
opt1["label"] = "God Mode";
opt1["enabled"] = false;
opt1["hotkey"] = "DPAD_UP";
menu["options"].push_back(opt1);

tinyjson::json opt2;
opt2["id"] = "infinite_ammo";
opt2["label"] = "Infinite Ammo";
opt2["enabled"] = false;
opt2["hotkey"] = "DPAD_DOWN";
menu["options"].push_back(opt2);

// Access with defaults
std::string title = menu.value<std::string>("title", "Default Menu");
bool enabled = menu.value_at_path<bool>("options.0.enabled", false);

// Modify via path
menu.set_path("options.0.enabled", tinyjson::json(true));
```

## 📚 API Reference

### Construction & Types

```cpp
tinyjson::json obj;                    // null
tinyjson::json str("text");            // string
tinyjson::json num(42);                // integer
tinyjson::json flt(3.14);              // float
tinyjson::json flag(true);             // boolean
```

### Type Checking

```cpp
bool is_null() const;
bool is_boolean() const;
bool is_number() const;
bool is_string() const;
bool is_array() const;
bool is_object() const;
```

### Value Access

```cpp
bool get_bool() const;
long long get_int() const;
double get_float() const;
const std::string& get_string() const;
```

### Safe Access

```cpp
template<typename T>
T value(const std::string& key, const T& default_val) const;

template<typename T>
T value_at_path(const std::string& path, const T& default_val) const;
```

### Path Operations

```cpp
json& at_path(const std::string& path);
const json& at_path(const std::string& path) const;
bool has_path(const std::string& path) const;
void set_path(const std::string& path, const json& value);
```

### Object Operations

```cpp
json& operator[](const std::string& key);
json& at(const std::string& key);
bool contains(const std::string& key) const;
bool erase(const std::string& key);
```

### Array Operations

```cpp
json& operator[](size_t index);
json& at(size_t index);
void push_back(const json& value);
size_t size() const;
bool empty() const;
```

### Serialization

```cpp
std::string dump(int indent = -1) const;
static json parse(const std::string& str);
```

### File I/O

```cpp
bool save_to_file(const std::string& filepath, int indent = 2) const;
bool save_to_file_verbose(const std::string& filepath, int indent, std::string& error_msg) const;
static json load_from_file(const std::string& filepath);
static json load_from_file_verbose(const std::string& filepath, std::string& error_msg);
```

### Iteration

```cpp
iterator begin();
iterator end();
const_iterator begin() const;
const_iterator end() const;
iterator find(const std::string& key);
```

## 💡 Examples

### Game Configuration System

```cpp
#include "Json.h"

void load_game_config() {
    tinyjson::json config;
    
    try {
        config = tinyjson::json::load_from_file("game:\\config.json");
    } catch (...) {
        // Use defaults if file doesn't exist
        config["graphics"]["quality"] = "Medium";
        config["audio"]["volume"] = 50;
    }
    
    // Safe access with defaults
    std::string quality = config.value_at_path<std::string>("graphics.quality", "Low");
    int volume = config.value_at_path<int>("audio.volume", 50);
    bool vsync = config.value_at_path<bool>("graphics.vsync", true);
    
    apply_settings(quality, volume, vsync);
}
```

### Dynamic Menu System

```cpp
tinyjson::json create_menu() {
    tinyjson::json menu;
    menu["title"] = "Mod Menu";
    menu["selected_index"] = 0;
    
    // Add menu items
    const char* labels[] = {"God Mode", "Infinite Ammo", "Super Speed", "Teleport"};
    const char* hotkeys[] = {"F1", "F2", "F3", "F4"};
    
    for (int i = 0; i < 4; i++) {
        tinyjson::json item;
        item["label"] = labels[i];
        item["hotkey"] = hotkeys[i];
        item["enabled"] = false;
        menu["items"].push_back(item);
    }
    
    return menu;
}

void toggle_menu_item(tinyjson::json& menu, int index) {
    // Build path
    char path[64];
    sprintf(path, "items.%d.enabled", index);
    
    // Get current value
    bool current = menu.value_at_path<bool>(path, false);
    
    // Toggle
    menu.set_path(path, tinyjson::json(!current));
}
```

### Player Save System

```cpp
void save_player_data(const PlayerData& player) {
    tinyjson::json save;
    
    // Basic info
    save["name"] = player.name;
    save["level"] = player.level;
    save["xp"] = player.experience;
    
    // Stats
    save["stats"]["health"] = player.maxHealth;
    save["stats"]["mana"] = player.maxMana;
    save["stats"]["strength"] = player.strength;
    
    // Inventory
    for (size_t i = 0; i < player.inventory.size(); i++) {
        tinyjson::json item;
        item["id"] = player.inventory[i].id;
        item["quantity"] = player.inventory[i].quantity;
        save["inventory"].push_back(item);
    }
    
    // Save to file
    save.save_to_file("game:\\save_data.json", 2);
}

PlayerData load_player_data() {
    PlayerData player;
    
    try {
        tinyjson::json save = tinyjson::json::load_from_file("game:\\save_data.json");
        
        player.name = save.value<std::string>("name", "Player");
        player.level = save.value<int>("level", 1);
        player.experience = save.value<int>("xp", 0);
        
        player.maxHealth = save.value_at_path<int>("stats.health", 100);
        player.maxMana = save.value_at_path<int>("stats.mana", 50);
        player.strength = save.value_at_path<int>("stats.strength", 10);
        
        // Load inventory...
    } catch (...) {
        // Return default player if load fails
    }
    
    return player;
}
```

### Network Message Serialization

```cpp
std::string create_network_message(const std::string& type, const std::string& data) {
    tinyjson::json msg;
    msg["type"] = type;
    msg["timestamp"] = get_current_time();
    msg["data"] = data;
    
    // Compact format for network
    return msg.dump(-1);
}

void parse_network_message(const std::string& json_str) {
    try {
        tinyjson::json msg = tinyjson::json::parse(json_str);
        
        std::string type = msg.value<std::string>("type", "unknown");
        std::string data = msg.value<std::string>("data", "");
        
        handle_message(type, data);
    } catch (const tinyjson::parse_error& e) {
        log_error("Invalid message format");
    }
}
```

## 🔨 Building

### Standard C++ Project

```bash
g++ -std=c++98 your_project.cpp -o your_project
```

### Xbox 360 Project

1. Add `Json.h` to your Xbox 360 project
2. Include in your source files: `#include "Json.h"`
3. Build normally with Xbox 360 SDK compiler

### Visual Studio

Simply add `Json.h` to your project and include it. Works with Visual Studio 2010 and later.

## 📄 License

MIT License

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

### Development Setup

1. Clone the repository
2. Make your changes
3. Test with Xbox 360 compatible compiler (or Visual Studio 2010)
4. Submit a pull request

### Guidelines

- Maintain Xbox 360 compatibility (no C++11 features)
- Keep the header-only design
- Add tests for new features
- Update documentation

## 🐛 Known Issues

- Subdirectory paths may not work on Xbox 360 (`game:\\folder\\file.json`)
  - Workaround: Use root paths like `game:\\file.json`
- Very large JSON files may cause memory issues on Xbox 360
  - Workaround: Use compact serialization `dump(-1)` and keep structures small

## 🙏 Acknowledgments

- Inspired by [nlohmann/json](https://github.com/nlohmann/json) but redesigned for Xbox 360
- Built for the xbNetwork modding project
- Optimized for game development and embedded systems

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/tinyjson/issues)
- **Documentation**: This README and code comments
- **Examples**: See `examples/` directory

---

Made with ❤️ for game modders and Xbox 360 developers
