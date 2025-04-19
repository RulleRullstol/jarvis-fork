
#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include "../libs/INIReader/INIReader.h"
#include <string>
#include <vector>
#include <filesystem>

class ConfigHandler {
private:
  INIReader reader;
  std::string getPath(const std::string &filename);


public:
  ConfigHandler(const std::string &filename = "config.ini");
  std::vector<std::string> getSections();
  std::vector<std::string> getKeys(const std::string &section);
  std::string getValue(const std::string &section, const std::string &key);
  std::vector<std::string> getLights();
  std::vector<std::string> findSections(const std::string &name);
  std::string getHAUrl();
  std::string getHAToken();
  std::string getStandardEmail();
  std::string getEmailPswd();
  std::string getGender();
};

#endif
