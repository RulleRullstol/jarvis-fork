#include "agent.h"
#include "tools/tools.h"

// tool_choice = "none", "auto", "required"
Agent::Agent(message sysMsg, bool useTools, string tool_choice) {
  fetchConfig();
  systemMsg = sysMsg;

  useTools = useTools;
    if (useTools) {
      toolChoice = "auto";
      tools = Tools::getToolDef(); 
      // tools;
    } else {
      toolChoice = "none";
      // tools;
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

void Agent::addHistory(message msg) {
  if (!msg.content.empty()) {
    while (history.size() >= maxHistory)
      history.erase(history.begin());
    history.insert(history.end(), msg);
  }
}

// str -> Json
Json::Value Agent::resBodyToJson(string str) {
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

message Agent::getResMessage(const Json::Value& res) {
  message msg;

  if (!res.isMember("choices") || !res["choices"].isArray() || res["choices"].empty()) {
    cerr << "Warning: No choices array in response." << endl;
    return msg;
  }

  const Json::Value& choices = res["choices"];
  const Json::Value& latest = choices[choices.size() -1];
  if (!latest.isMember("message") || !latest["message"].isObject()) {
    cerr << "Warning: No message object in choice." << endl;
    return msg;
  }

  const Json::Value& messageObj = latest["message"];
  msg.role = messageObj.get("role", "").asString();
  msg.content = messageObj.get("content", "").asString();

  return msg;
}



// req -> CurlPost. Retrunerar tom Json om nåt gått snett
Json::Value Agent::query(message msg) {
  addHistory(msg);
  // Skapa requestt
  request req;

  req.model = model;
  req.max_tokens = maxTokens;
  //req.tool_choice = toolChoice;

  req.messages.clear();
  req.messages.push_back(systemMsg);
  req.messages.insert(req.messages.end(), history.begin(), history.end());
  //req.tools = tools;

  // Post
  vector<string> headers = {"Content-Type: application/json",
                            "Authorization: Bearer " + token};
  recursionCounter = -1; // Debug
  Json:Json::Value reqJson = structToJson(req);
  // Ananrs blir openai arg
  if (!useTools) {
    reqJson.removeMember("tools");
    reqJson.removeMember("tool_choice");
  }

  string body = Json::FastWriter().write(reqJson);

  //cout << "Debug: Recursion counter = " << recursionCounter << endl;
  string response = crl.post(apiUrl, headers, body);
  Json::Value jsonRes = resBodyToJson(response);
  // Debug
  message resMsg = getResMessage(jsonRes);
  // Stoppa in svar i history
  addHistory(resMsg);

  return jsonRes;
}
