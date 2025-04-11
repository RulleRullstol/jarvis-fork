#ifndef AGENT_H
#define AGENT_H
#include "../utils/configHandler.h"
#include "../utils/env/envParser.h"
#include "../utils/web/webUtils.h"
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <string>
#include <vector>

using namespace std;

// macro för message struct
#define messageMacro(Structname)                                               \
  struct Structname {                                                          \
    string role;                                                               \
    string content;                                                            \
  };
messageMacro(message);

// macro for API completionResponse. Det vi behöver
#define requestMacro(StructName)                                               \
  struct StructName {                                                          \
    string model;                                                              \
    int max_tokens;                                                            \
    vector<message> messages;                                                  \
    Json::Value tools; /*[{}]*/                                                \
    string tool_choice;                                                        \
  };

// Skapa struct
requestMacro(request);

class Agent {
private:
  CurlPost crl;
  int maxTokens;
  int maxHistory;
  message systemMsg;
  string model;
  string toolChoice;
  Json::Value tools;
  vector<message> history;

  // env & nät grejs
  string apiUrl;
  string token;

  Json::Value requestToJson(request req);
  Json::Value msgToJson(message msg);
  Json::Value strToJson(string str);
  void addHistory(message msg);
  void fetchConfig();

public:
  Agent(message sysMsg, bool useTools, string tool_choice = "none");
  ~Agent();

  Json::Value query(message msg);
};

#endif // !#ifndef AGENT_H
