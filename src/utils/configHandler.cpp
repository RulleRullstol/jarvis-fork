#include "../../libs/INIReader/INIReader.h"
#include <iostream>
#include <string>

using namespace std;

class ConfigHandler {
    private: INIReader reader;

    public:
    ConfigHandler(const string& filename = "../../includes/config.ini") : reader(filename) {
        if (reader.ParseError() != 0) {
            cerr << "Error opening config.ini" << endl;
        }
        cout << "Config file loaded successfully." << endl;
    }

    // Returns all sections
    vector<string> getSections() {
        return reader.Sections();
    }

    // Returns the value of the key in the section
    string getValue(const string& section, const string& key) {
        return reader.Get(section, key, "");
    }

    // Returns all key-value pairs in section
    vector<string> getKeys(const string& section) {
        return reader.Keys(section);
    }
};




