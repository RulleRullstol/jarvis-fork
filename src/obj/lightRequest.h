
#include <string>

#ifndef LIGHT_H
#define LIGHT_H

struct LightRequest {
    std::string id;
    bool state;
    int brightness;
    int rgb[3];
};

#endif