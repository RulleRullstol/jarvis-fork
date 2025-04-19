#include "tools.h"
#include "../../utils/configHandler.h"

Json::Value Tools::getToolDef() {
  string json = R"(
    [{"type": "function",
        "function": {
            "name": "setLights",
            "description": "Sets the properties of lights.",
            "parameters": {
                "type": "object",
                    "properties": {
                    "id": {
                        "type": "string",
                        "description": "The id of the light to be altered. The available IDS are: LIGHT_NAMES
                         If no light is specified set id to 'all'. 
                        if the user specifies no particular color set the color value to none"
                    },
                    "state": {
                        "type": "boolean",
                        "description": "The on/off state of the light"
                    },
                    "color": {
                        "type": "array",
                        "description": "The RGB color value of the light represented as the aray [R, G, B], each element is an integer and have a range of 0-255",
                        "items": {
                            "type": "number"
                        }
                    },
                    "brightness": {
                        "type": "number",
                        "description": "The brightness of the light, takes a value in the range 0-255"
                    },
                },
                "required": ["id", "state", "color", "brightness"]
            }
        }
    }, {"type": "function",
        "function": {
            "name": "sendManual",
            "description": "Sends a manual to the user via email"
        }}];)";
  
  string lightNames;
  string light;

  ConfigHandler conf;
  vector<string> lightSections = conf.getLights();
  for (string sec : lightSections)
    lightNames.append(conf.getValue(sec, "ha_id") + ", ");


  // Byt ut placeholder med data från config
  size_t pos;
  while ((pos = json.find("LIGHT_NAMES")) != string::npos)
    json.replace(pos, 11, lightNames);

  while ((pos = json.find("MANUAL_NAMES")) != string::npos)
    json.replace(pos, 12, "manual");

  return strToJson(json);
}
