
#include "lightHandler.h"
#include "../openai/agent.h"
#include "configHandler.h"
#include "web/webUtils.h"
#include <array>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>

using namespace std;

ConfigHandler ch = ConfigHandler();
vector<string> lights = ch.getLights();
CurlPost cp = CurlPost();

array<bool, 3> validateList(LightRequest &lr, string &l) {
  // Validation Checklist
  bool id = false;
  bool brightness = false;
  bool color = false;

  // id check
  for (string light : lights) {
    if (ch.getValue(light, "ha_id") == lr.id) {
      id = true;
      l = light;
      break;
    }
  }

  // brightness check
  if (ch.getValue(l, "brightness") == "true") {
    if (lr.brightness >= 0 && lr.brightness <= 255)
      brightness = true;
  }

  // color check
  if (ch.getValue(l, "color") == "true") {
    if (lr.rgb[0] >= 0 && lr.rgb[0] <= 255 && lr.rgb[1] >= 0 &&
        lr.rgb[1] <= 255 && lr.rgb[2] >= 0 && lr.rgb[2] <= 255) {
      color = true;
    }
  }

  return {id, brightness, color};
}

void setLights(vector<LightRequest> &lrV) {
  cout << lrV[0].id << endl;
  for (LightRequest lr : lrV) {
    string l = "";
    array<bool, 3> validation = validateList(lr, l);
    if (validation[0] == false)
      return;

    Json::Value payload;

    payload["entity_id"] = lr.id;

    vector<string> headers = {"Authorization: " + ch.getHAToken(),
                              "content-type: application/json"};

    if (lr.state) {
      if (validation[1])
        payload["brightness"] = to_string(lr.brightness);

      Json::Value rgbArray(Json::arrayValue);
      if (validation[2]) {
        for (int i = 0; i <= 2; i++) {
          rgbArray.append(lr.rgb[i]);
        }
        payload["rgb_color"] = rgbArray;
      }

      string url = ch.getHAUrl() + ch.getValue(l, "uri_on");

      string pl = jsonToString(payload, false);
      cout << headers[0] << endl
           << headers[1] << endl
           << url << endl
           << pl << endl;

      string debug = cp.post(url, headers, pl);

      cout << debug << endl;
    } else {
      string url = ch.getHAUrl() + ch.getValue(l, "uri_off");

      string pl = jsonToString(payload, false);

      string debug = cp.post(url, headers, pl);
      cout << debug << endl;
    }
  }

  lrV.clear();
}
