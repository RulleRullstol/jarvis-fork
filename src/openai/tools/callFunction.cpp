#include "tools.h"

// callFunction tar Json::Value av tool_calls i response
void Tools::callFunction(Json::Value toolCalls) {
  for (const Json::Value call : toolCalls) {
    string funcName = toolCalls["function"]["name"].asString();
    Json::Value args = toolCalls["function"]["arguments"];
    
    auto funcIt = functionMap.find(funcName);
    if (funcIt != functionMap.end())
      funcIt->second(args);
  }
};
