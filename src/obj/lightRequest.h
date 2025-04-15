
#include <array>
#include <string>
#ifndef LIGHT_H
#define LIGHT_H

struct LightRequest {
  std::string id;
  bool state;
  int brightness;
  std::array<int, 3> rgb;
};

#endif
