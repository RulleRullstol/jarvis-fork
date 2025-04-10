#ifndef AGENT_H
#define AGENT_H
#include "../utils/configHandler.h"
#include "../utils/web/webUtils.h"
#include <string>
#include <vector>
// #include <json snusk>

using namespace std;

struct responseBody {
  // https://platform.openai.com/docs/api-reference/chat/create
};

class Agent {
private:
  int maxTokens;
  int maxHistory;
  string systemMsg;
  vector<string> tools;
  vector<string> history;

  string constructPrompt(const string &prompt);
  responseBody parseResponse(const string &response);

public:
  Agent();
  ~Agent();

  string query(string prompt);
};

#endif // !#ifndef AGENT_H
#define AGENT_H
