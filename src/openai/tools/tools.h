#ifndef TOOLS_H
#define TOOLS_H
#include <functional>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "../../utils/lightHandler.h"
#include "../../obj/lightRequest.h"

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
  unordered_map<string, function<void(const Json::Value&)>> functionMap = {
    {"setLights", [](const Json::Value& args){
      vector<LightRequest> lrList;
      for (const auto& lrJson : args) {
        LightRequest lr;
        lr.id = lrJson["id"].asString();
        lr.brightness = lrJson["brightness"].asInt();
        lr.state = lrJson["state"].asBool();
        
        array<int, 3> lrRgb;
        lrRgb[0] = lrJson["rgb"][0].asInt();
        lrRgb[1] = lrJson["rgb"][1].asInt();
        lrRgb[2] = lrJson["rgb"][2].asInt();

        lr.rgb = lrRgb;
        lrList.push_back(lr);
      }
    setLights(lrList);
    }}
  };
  public:
  void callFunction(Json::Value toolCalls);
  static Json::Value getToolDef();
};

#endif
