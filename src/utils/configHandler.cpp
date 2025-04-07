#include "../../libs/INIReader/INIReader.h"
#include <iostream>
#include <string>

class ConfigHandler {
    private: INIReader reader;

    public:
    ConfigHandler(const std::string& filename = "../../includes/config.ini")
        : reader(filename) {
        if (reader.ParseError() != 0) {
            std::cerr << "Error opening config.ini" << std::endl;
        }
    }

    // Returns the value of the key in the section
    std::string getValue(const std::string& section, const std::string& key) {
        return reader.Get(section, key, "");
    }

    // Returns all key-value pairs in section
    std::string getSection(const std::string& section) {
        return reader.Get(section, "", "");
    }
};



