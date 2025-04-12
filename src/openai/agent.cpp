#include "agent.h"

// tool_choice = "none", "auto", "required"
Agent::Agent(message sysMsg, bool useTools, string tool_choice) {
  fetchConfig();
  systemMsg = sysMsg;
  /*
    if (useTools) {
      toolChoice = tool_choice;
      // tools;
    } else {
      toolChoice = "none";
      // tools;
    }*/
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

void Agent::addHistory(message msg) {
  if (history.size() >= maxHistory)
    history.erase(history.begin());
  history.push_back(msg);
}

// str -> Json
Json::Value resBodyToJson(string str) {
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

message Agent::getResMessage(Json::Value res) {
  message msg;
  try {
    msg.role = res["choices"][0]["message"]["role"].asString();
    msg.content = res["choices"][0]["message"]["content"].asString();
  } catch (const exception &e) {
    cout << "Error while parsing message from response: " << e.what() << endl;
  }
  return msg;
}

// req -> CurlPost. Retrunerar tom Json om nåt gått snett
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
  // req.tools = tools;

  // Post
  vector<string> headers = {"Content-Type: application/json",
                            "Authorization: Bearer " + token};
  string body = Json::FastWriter().write(
      structToJson(req)); // Denna fungerar bara för structs i reflect<>
  string response = crl.post(apiUrl, headers, body);
  Json::Value jsonRes = resBodyToJson(response);

  // Stoppa in svar i history
  history.push_back(getResMessage(jsonRes));

  return jsonRes;
}
