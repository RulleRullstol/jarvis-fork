#ifndef AGENT_H
#define AGENT_H
#include "../utils/configHandler.h"
#include "../utils/env/envParser.h"
#include "../utils/web/webUtils.h"
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

struct message {
  string role;
  string content;
};

struct request {
  string model;
  int max_tokens;
  vector<message> messages;
  Json::Value tools; /*[{}]*/
  string tool_choice;
};

// structs to relfect message and request
template <typename T> struct reflect;

template <> struct reflect<message> {
  using T = message;
  static constexpr auto fields = make_tuple(make_pair("role", &T::role),
                                            make_pair("content", &T::content));
};

template <> struct reflect<request> {
  using T = request;
  static constexpr auto fields = make_tuple(
      make_pair("model", &T::model), make_pair("max_tokens", &T::max_tokens),
      make_pair("messages", &T::messages), make_pair("tools", &T::tools),
      make_pair("tool_choice", &T::tool_choice));
};

template <typename T>
using hasReflect = decltype(reflect<T>::fields,
                            void()); // Testa läsa typen på reflect::field. Om
                                     // definierad: decltype lyckas; -> void
                                     // Annars

template <typename T> struct reflectable {
  static constexpr bool value =
      is_same_v<hasReflect<T>,
                void>; // Kolla om hasReflect ger void isf true. Om decltype
                       // misslyckas kommer is_same ge false.
};

// struct -> json genom att rekursera genom nested structs. Kan bara ta structs
// definierade i reflect<>
template <typename T> Json::Value structToJson(T &obj) {
  Json::Value json;

  // Kolla om structen har en definerad reflect template
  if constexpr (reflectable<T>::value) {
    for (const auto &[name, ptr] : reflect<T>::fields) {
      const auto &value = obj.*ptr;
      // Kolla om värdet i template tupeln har en reflect spec
      if constexpr (reflectable<decay_t<decltype(value)>>::value)
        json[name] = structToJson(value); // Kalla funktion igen
      else
        json[name] = value;
    }
  } else {
    static_assert(reflectable<T>::value,
                  "Type not reflectable. Cannot convert to JSON");
  }
  return json;
}

static string jsonToString(const Json::Value &json) {
  Json::StreamWriterBuilder builder;
  builder["indentation"] = "";
  return Json::writeString(builder, json);
}

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

  Json::Value resBodyToJson(string str);
  message getResMessage(Json::Value res);
  void addHistory(message msg);
  void fetchConfig();

public:
  Agent(message sysMsg, bool useTools, string tool_choice = "none");
  ~Agent();

  Json::Value query(message msg);
};

#endif // !#ifndef AGENT_H
