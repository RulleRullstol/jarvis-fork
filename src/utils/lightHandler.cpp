
#include "lightHandler.h"
#include "configHandler.h"
#include <cstddef>
#include <vector>
#include <string>

using namespace std;

ConfigHandler *ch = new ConfigHandler();
vector<string> lights = ch->getLights();

void setLigts(LightRequest &lr) {
    if (!validateList(lr))
        return;
    
}

bool validateList(LightRequest &lr) {
    // Validation Checklist
    bool id = false;
    bool brightness = false;
    bool color = false;

    string l;

    // id check
    for (string light : lights) {
        if (ch->getValue(light, "ha_id") == lr.id) {
            id = true;
            l = light;
            break;
        }
    }

    // brightness check
    if (ch->getValue(l, "brightness") == "true") {
        if (lr.brightness >= 0 && lr.brightness <= 255)
            brightness = true;
    } else
        lr.brightness = NULL;


    // color check
    if (ch->getValue(l, "color") == "true") {
        if (lr.rgb[0] >= 0 && lr.rgb[0] <= 255 &&
            lr.rgb[1] >= 0 && lr.rgb[1] <= 255 &&
            lr.rgb[2] >= 0 && lr.rgb[2] <= 255) {
            color = true;
        }
    } else {
        for (int i = 0; i < 3; i++)
            lr.rgb[i] = NULL;
    }

    return id && brightness && color;
}