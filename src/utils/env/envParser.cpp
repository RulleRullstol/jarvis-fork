#include "envParser.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

// env variabler i filen
static unordered_map<string, string> envMap;

static void loadEnvFile(const string &filename) {
  ifstream envFile(filename);
  if (!envFile.is_open()) {
    cerr << "Could not open the file " << filename << endl;
    return;
  }

  string line;
  while (getline(envFile, line)) {
    // Ignore empty lines or comments
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Hitta =
    size_t equalPos = line.find('=');
    if (equalPos != string::npos) {
      string key = line.substr(0, equalPos);
      string value = line.substr(equalPos + 1);

      // Remove newlines and stuff
      key.erase(key.find_last_not_of(" \t\n\r") + 1);
      value.erase(value.find_last_not_of(" \t\n\r") + 1);

      envMap[key] = value;
    }
  }
  envFile.close();
}
// Returns empty string if not found
string getEnvValue(const string &key, const string &envFilePath) {
  if (envMap.empty()) {
    loadEnvFile(envFilePath);
  }

  auto it = envMap.find(key);
  if (it != envMap.end()) {
    return it->second;
  } else {
    cout << "Failed to find env: " << key << "Returning empty str" << endl;
    return "";
  }
}
