#include "configHandler.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>

using namespace std;

ConfigHandler::ConfigHandler(const string &filename) : reader(getPath(filename)) {
  if (reader.ParseError() != 0) {
    cerr << "Error opening config.ini" << endl;
  } else
    cout << "Config file loaded successfully." << endl;
}

string ConfigHandler::getPath(const string &filename) {
  string path;
  filesystem::path dir = filesystem::current_path();
  cout << "Looking for: " << filename << endl;
  while (path.empty()) {
    for (const auto& entry : filesystem::recursive_directory_iterator(dir)) {
      if (entry.path().filename() == filename) {
        cout << "Found: " << entry.path() << endl;
        path = entry.path().string();
        break;
      }
    }
    if (path.empty())
      dir = dir.parent_path();
  }
  if (path.empty())
    throw runtime_error("Could not load config");
  return path;
}

vector<string> ConfigHandler::getSections() { return reader.Sections(); }

vector<string> ConfigHandler::getKeys(const string &section) {
  return reader.Keys(section);
}

string ConfigHandler::getValue(const string &section, const string &key) {
  return reader.Get(section, key, "");
}

vector<string> ConfigHandler::getLights() {
  vector<string> lights;
  vector<string> sections = getSections();
  for (const string &sec : sections) {
    if (sec.find("light") != string::npos) {
      lights.push_back(sec);
    }
  }
  return lights;
}

// Returns sections containing the str passed
vector<string> ConfigHandler::findSections(const string &name) {
  vector<string> foundSecs;
  vector<string> sections = ConfigHandler::getSections();
  for (const string &secName : sections) {
    if (secName.find(name))
      foundSecs.push_back(secName);
  }
  return foundSecs;
}

string ConfigHandler::getHAUrl() {
  return getValue("home_assistant_settings", "url");
}

string ConfigHandler::getHAToken() {
  return "Bearer " + getValue("home_assistant_settings", "token");
}

string ConfigHandler::getStandardEmail() { return getValue("email", "mail"); }

string ConfigHandler::getEmailPswd() { return getValue("email", "apppwd"); }

string ConfigHandler::getGender() { return getValue("personal", "gender"); }
