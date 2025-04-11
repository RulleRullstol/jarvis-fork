#include "agent.h"

// tool_choice = "none", "auto", "required"
Agent::Agent(message sysMsg, bool useTools, string tool_choice) {
  fetchConfig();
  systemMsg = sysMsg;

  if (useTools) {
    toolChoice = tool_choice;
    tools = Json::Value();
  } else {
    toolChoice = "none";
    tools = NULL;
  }
}

Agent::~Agent() { return; }

// greppa env och annat svammel
void Agent::fetchConfig() {
  ConfigHandler conf;
  maxTokens = stoi(conf.getValue("llm_settings", "max_tokens"));
  maxHistory = stoi(conf.getValue("llm_settings", "memory"));
  model = conf.getValue("llm_settings", "model");
  apiUrl = conf.getValue("llm_settings", "api_url");

  // Greppa openAI token
  token = getEnvValue("OPENAI_API_KEY", "../../includes/.env");
  if (!token.compare(""))
    cerr << "Agent failed to load API key" << endl;
}

Json::Value msgToJson(message msg) {
  Json::Value msgJson;
  msgJson["role"] = msg.role;
  msgJson["content"] = msg.content;
  return msgJson;
}

Json::Value Agent::requestToJson(request req) {
  Json::Value jsonReq;
  jsonReq["model"] = req.model;
  jsonReq["max_tokens"] = req.max_tokens;
  jsonReq["tool_choice"] = toolChoice;

  for (const auto &msg : req.messages)
    jsonReq["messages"].append(msgToJson(msg));

  jsonReq["tools"] = tools;
  return jsonReq;
}

void Agent::addHistory(message msg) {
  if (history.size() >= maxHistory)
    history.erase(history.begin());
  history.push_back(msg);
}

// str -> Json
Json::Value strToJson(string str) {
  Json::Value strJson;
  Json::CharReaderBuilder reader;
  string errs;

  istringstream s(str);
  if (!Json::parseFromStream(reader, s, &strJson, &errs)) {
    cout << "Failed to parse JSON: " << errs << endl;
    return Json::Value();
  }
  return strJson;
}

// req -> CurlPost
Json::Value Agent::query(message msg) {
  addHistory(msg);
  // Skapa requestt
  request req;
  req.model = model;
  req.max_tokens = maxTokens;
  req.tool_choice = toolChoice;

  // meddelanden & tools
  req.messages = history;
  req.messages.push_back(systemMsg);
  req.tools = tools;

  // Post
  vector<string> headers = {"Content-Type: application/json",
                            "Authorization: Bearer " + token};
  string body = Json::FastWriter().write(requestToJson(req));
  string response = crl.post(apiUrl, headers, body);

  return strToJson(response);
}
