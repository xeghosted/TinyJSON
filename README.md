# nlohmann::json for Xbox 360

A lightweight, self-contained JSON library implementation for Xbox 360 C++ development. This library provides a simple and intuitive interface for parsing, creating, and manipulating JSON data without external dependencies.

## Features

- **Zero Dependencies**: Self-contained single-header implementation
- **Xbox 360 Compatible**: Designed specifically for Xbox 360 C++ development
- **Intuitive API**: Clean, modern C++ interface similar to nlohmann/json
- **Full JSON Support**: Parse and serialize all JSON types (null, boolean, number, string, array, object)
- **Type Safety**: Built-in type checking with exception handling
- **STL Integration**: Uses standard library containers (std::map, std::vector, std::string)

## Supported JSON Types

- `null` - Null values
- `boolean` - true/false values
- `number_integer` - Integer numbers (long long)
- `number_float` - Floating-point numbers (double)
- `string` - String values
- `array` - Ordered collections
- `object` - Key-value maps

## Basic Usage

### Including the Library

```cpp
#include "json.hpp"

using json = nlohmann::json;
```

### Creating JSON Objects

```cpp
// Create different JSON types
json null_value = nullptr;
json bool_value = true;
json int_value = 42;
json float_value = 3.14;
json string_value = "Hello, Xbox 360!";

// Create an object
json obj;
obj["name"] = "Player1";
obj["score"] = 1000;
obj["active"] = true;

// Create an array
json arr;
arr.push_back(1);
arr.push_back(2);
arr.push_back(3);
```

### Parsing JSON Strings

```cpp
// Parse from string
std::string json_string = R"({"name":"Xbox","version":360})";
json parsed = json::parse(json_string);

std::string name = parsed["name"].get_string();  // "Xbox"
int version = parsed["version"].get_int();       // 360

// Parse from stream
std::ifstream file("config.json");
json config = json::parse(file);
```

### Accessing Values

```cpp
json data = json::parse(R"({
    "player": {
        "name": "MasterChief",
        "level": 10,
        "inventory": ["weapon", "shield", "grenade"]
    }
})");

// Object access
std::string name = data["player"]["name"].get_string();

// Array access
json inventory = data["player"]["inventory"];
std::string item = inventory[0].get_string();  // "weapon"

// Safe access with at()
try {
    json& level = data.at("player").at("level");
} catch (const nlohmann::parse_error& e) {
    // Handle missing key
}
```

### Type Checking

```cpp
json value = 42;

if (value.is_number()) {
    int num = value.get_int();
}

if (value.is_string()) {
    std::string str = value.get_string();
}

if (value.is_object()) {
    // Iterate over object
    for (auto it = value.begin(); it != value.end(); ++it) {
        std::string key = it->first;
        json& val = it->second;
    }
}

if (value.is_array()) {
    size_t length = value.size();
}
```

### Serialization

```cpp
json data;
data["game"] = "Halo 3";
data["year"] = 2007;

// Compact output
std::string compact = data.dump();
// {"game":"Halo 3","year":2007}

// Pretty-printed output
std::string pretty = data.dump(4);
// {
//     "game": "Halo 3",
//     "year": 2007
// }
```

### Building Complex Structures

```cpp
json game_state;

// Player data
game_state["player"]["health"] = 100;
game_state["player"]["position"]["x"] = 10.5;
game_state["player"]["position"]["y"] = 20.3;
game_state["player"]["position"]["z"] = 5.0;

// Inventory array
game_state["player"]["inventory"] = json();
game_state["player"]["inventory"].push_back("assault_rifle");
game_state["player"]["inventory"].push_back("plasma_grenade");

// Enemy array
json enemies;
for (int i = 0; i < 3; i++) {
    json enemy;
    enemy["id"] = i;
    enemy["type"] = "grunt";
    enemy["health"] = 50;
    enemies.push_back(enemy);
}
game_state["enemies"] = enemies;

// Save to file
std::ofstream file("save_game.json");
file << game_state.dump(2);
```

## Helper Functions

The library includes template helper functions for safe value extraction with default values:

```cpp
json config = json::parse(file);

// Get values with defaults
std::string username = JsonGet<std::string>(config, "username", "Guest");
int max_players = JsonGet<int>(config, "max_players", 4);
double volume = JsonGet<double>(config, "volume", 0.75);
bool fullscreen = JsonGet<bool>(config, "fullscreen", true);
long long score = JsonGet<long long>(config, "high_score", 0LL);

// If key doesn't exist or type doesn't match, returns default value
```

## Exception Handling

The library throws exceptions for error conditions:

```cpp
try {
    json data = json::parse(invalid_json_string);
    int value = data["key"].get_int();
} catch (const nlohmann::parse_error& e) {
    // Handle parsing errors
    std::cout << "Parse error: " << e.what() << std::endl;
} catch (const nlohmann::type_error& e) {
    // Handle type errors
    std::cout << "Type error: " << e.what() << std::endl;
} catch (const nlohmann::out_of_range& e) {
    // Handle out of range errors
    std::cout << "Out of range: " << e.what() << std::endl;
}
```

## Common Use Cases for Xbox 360

### Configuration Files

```cpp
// Load game configuration
std::ifstream config_file("game_config.json");
json config = json::parse(config_file);

float music_volume = JsonGet<double>(config, "music_volume", 0.8);
float sfx_volume = JsonGet<double>(config, "sfx_volume", 1.0);
bool invert_y = JsonGet<bool>(config, "invert_y_axis", false);
```

### Save Game Data

```cpp
// Create save game
json save_data;
save_data["checkpoint"] = "mission_03";
save_data["playtime"] = 3600;
save_data["difficulty"] = "heroic";
save_data["completed_missions"] = json();
save_data["completed_missions"].push_back("mission_01");
save_data["completed_missions"].push_back("mission_02");

// Write to file
std::ofstream save_file("save_slot_1.json");
save_file << save_data.dump(2);
```

### Network Data Exchange

```cpp
// Parse server response
std::string server_response = ReceiveFromServer();
json response = json::parse(server_response);

if (response.contains("status") && response["status"].get_string() == "success") {
    json player_data = response["data"];
    // Process player data
}
```

## Performance Considerations

- The library uses dynamic memory allocation (new/delete) for strings, objects, and arrays
- For performance-critical sections, consider reusing json objects instead of creating new ones
- Large JSON files should be streamed when possible
- Pretty printing adds overhead; use compact mode for network transmission

## Limitations

- No support for JSON comments (not part of JSON standard)
- No support for Unicode escape sequences beyond basic escapes
- No direct support for custom types (use manual serialization)
- Array iteration uses indices, not iterators

## API Reference

### Type Checking Methods
- `is_null()` - Check if value is null
- `is_boolean()` - Check if value is boolean
- `is_number()` - Check if value is number (int or float)
- `is_string()` - Check if value is string
- `is_array()` - Check if value is array
- `is_object()` - Check if value is object

### Value Getter Methods
- `get_bool()` - Get boolean value
- `get_int()` - Get integer value (returns long long)
- `get_float()` - Get floating-point value (returns double)
- `get_string()` - Get string value (returns const std::string&)

### Object Methods
- `operator[](const std::string& key)` - Access/create object member
- `at(const std::string& key)` - Checked object access (throws if key missing)
- `contains(const std::string& key)` - Check if key exists
- `find(const std::string& key)` - Find key in object
- `begin()`, `end()` - Iterator access

### Array Methods
- `operator[](size_t index)` - Access/create array element
- `at(size_t index)` - Checked array access (throws if out of bounds)
- `push_back(const json& val)` - Add element to array
- `size()` - Get array/object/string size
- `empty()` - Check if empty

### Serialization Methods
- `dump(int indent = -1)` - Serialize to string (indent for pretty-print)
- `static parse(const std::string& str)` - Parse from string
- `static parse(std::istream& stream)` - Parse from stream

## License

This is a custom implementation for Xbox 360 development. Check with your project's licensing requirements.

## Example: Complete Game Configuration System

```cpp
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

class GameConfig {
private:
    json config;
    
public:
    bool Load(const std::string& filename) {
        try {
            std::ifstream file(filename);
            config = json::parse(file);
            return true;
        } catch (const nlohmann::parse_error& e) {
            std::cout << "Failed to load config: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool Save(const std::string& filename) {
        try {
            std::ofstream file(filename);
            file << config.dump(4);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Audio settings
    float GetMusicVolume() { return JsonGet<double>(config, "music_volume", 0.8); }
    void SetMusicVolume(float vol) { config["music_volume"] = vol; }
    
    float GetSFXVolume() { return JsonGet<double>(config, "sfx_volume", 1.0); }
    void SetSFXVolume(float vol) { config["sfx_volume"] = vol; }
    
    // Controls
    bool GetInvertY() { return JsonGet<bool>(config, "invert_y", false); }
    void SetInvertY(bool invert) { config["invert_y"] = invert; }
    
    int GetSensitivity() { return JsonGet<int>(config, "sensitivity", 5); }
    void SetSensitivity(int sens) { config["sensitivity"] = sens; }
};

int main() {
    GameConfig config;
    
    if (config.Load("settings.json")) {
        float music = config.GetMusicVolume();
        std::cout << "Music Volume: " << music << std::endl;
        
        // Modify and save
        config.SetMusicVolume(0.5);
        config.Save("settings.json");
    }
    
    return 0;
}
```

---

**Note**: This library is designed for Xbox 360 development and may require adjustments for other platforms.
