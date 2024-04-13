#ifndef __CONSTANT_LOADER_H__
#define __CONSTANT_LOADER_H__

#include <cugl/cugl.h>

using namespace cugl;

class ConstantsLoader {
private:
    std::unordered_map<std::string, std::variant<float, std::string>> constants;

    // Private constructor to prevent instantiation from outside
    ConstantsLoader() {}

public:
    // Static method to get the singleton instance
    static ConstantsLoader getInstance() {
        static ConstantsLoader instance;
        return instance;
    }

    // Getter method to access the unordered map
    std::unordered_map<std::string, std::variant<float, std::string>>& getMap() {
        return constants;
    }

    // Other methods to manipulate the unordered map can be added here

    // Getter method to access the float value from the map
    float getFloat(std::string key) {
        return std::get<float>(constants[key]);
    }

    // Getter method to access the string value from the map
    std::string getString(std::string key) {
        return std::get<std::string>(constants[key]);
    }
};

#endif /* __CONSTANT_LOADER_H__ */