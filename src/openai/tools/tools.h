#ifndef TOOLS_H
#define TOOLS_H
#include "../../obj/lightRequest.h"
#include "../../utils/lightHandler.h"
#include "../../utils/mailHandler.h"
#include "../agent.h"
#include <functional>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

static Json::Value strToJson(string jsonStr) {
  Json::CharReaderBuilder builder;
  Json::Value json;
  string errs;

  istringstream stream(jsonStr);
  if (!Json::parseFromStream(builder, stream, &json, &errs)) {
    cerr << "Error Parsing JSON" << errs << endl;
  }
  return json;
}

class Tools {
private:
  // map of functions. handling Json {args} => args
  unordered_map<string, function<void(const Json::Value &args)>> functionMap = {
      {"setLights",
       [](const Json::Value &args) {
         vector<LightRequest> lrList;

         LightRequest lr;
         lr.id = args["id"].asString();
         lr.brightness = args["brightness"].asInt();
         lr.state = args["state"].asBool();

         array<int, 3> lrRgb;
         lrRgb[0] = args["color"][0].asInt();
         lrRgb[1] = args["color"][1].asInt();
         lrRgb[2] = args["color"][2].asInt();

         lr.rgb = lrRgb;
         lrList.push_back(lr);
         thread([lrList]() { setLights(lrList); }).detach();
       }},
      {"sendManual", [](const Json::Value &args) {
         thread([]() { sendManual(); }).detach();
       }}};

public:
  void callFunction(Json::Value toolCalls);
  static Json::Value getToolDef();
};

#endif
