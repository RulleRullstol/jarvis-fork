#include "ConfigHandler.h"
#include <iostream>

using namespace std;

ConfigHandler::ConfigHandler(const string& filename) : reader(filename) {
    if (reader.ParseError() != 0) {
        cerr << "Error opening config.ini" << endl;
    }
    cout << "Config file loaded successfully." << endl;
}

vector<string> ConfigHandler::getSections() {
    return reader.Sections();
}

vector<string> ConfigHandler::getKeys(const string& section) {
    return reader.Keys(section);
}

string ConfigHandler::getValue(const string& section, const string& key) {
    return reader.Get(section, key, "");
}


vector<string> ConfigHandler::getLights() {
    vector<string> lights;
    vector<string> sections = getSections();
    for (const string& sec : sections) {
        if (sec.find("light") != string::npos) {
            lights.push_back(sec);
        }
    }
    return lights;
}
