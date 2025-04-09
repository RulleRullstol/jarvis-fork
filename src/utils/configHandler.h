
#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include "../../libs/INIReader/INIReader.h"
#include <vector>
#include <string>

class ConfigHandler {
private:
    INIReader reader;

public:
    ConfigHandler(const std::string& filename = "../../includes/config.ini");
    std::vector<std::string> getSections();
    std::vector<std::string> getKeys(const std::string& section);
    std::string getValue(const std::string& section, const std::string& key);
    std::vector<std::string> getLights();
    std::vector<std::string> findSections(const std::string& name);
};

#endif
