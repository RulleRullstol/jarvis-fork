#include "tools.h"

// callFunction tar Json::Value av tool_calls i response
void Tools::callFunction(Json::Value toolCalls) {
  for (const auto &call : toolCalls) {
    string funcName = call["function"]["name"].asString();
    Json::Value args = call["function"]["arguments"];

    auto funcIt = functionMap.find(funcName);
    if (funcIt != functionMap.end())
      funcIt->second(args);
  }
};
