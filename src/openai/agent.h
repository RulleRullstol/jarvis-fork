#ifndef AGENT_H
#define AGENT_H
#include "../utils/configHandler.h"
#include "../utils/env/envParser.h"
#include "../utils/web/webUtils.h"
#include <cstddef>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <string>
#include <tuple>
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
  // vector<Json::Value> tools; /*[{}]*/
  string tool_choice;
};

// structs to relfect message and request
template <typename T> struct reflect;

template <> struct reflect<message> {
  using T = message;
  static constexpr auto fields = make_tuple(
    make_pair("role", &T::role),
    make_pair("content", &T::content));
};

template <> struct reflect<request> {
  using T = request;
  static constexpr auto fields = make_tuple(
      make_pair("model", &T::model),
      make_pair("max_tokens", &T::max_tokens),
      // make_pair("messages", &T::messages), make_pair("tools", &T::tools),
      make_pair("tool_choice", &T::tool_choice));
};

template <typename T, typename = void> struct hasReflect : false_type {};

template <typename T>
struct hasReflect<T, std::void_t<decltype(reflect<std::decay_t<T>>::fields)>>: std::true_type {};
// definierad: decltype lyckas; -> true

template <typename T> struct reflectable {
  static constexpr bool value = hasReflect<T>::value;
};

template <typename T, template <typename...> class Template>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

// compile time loop för att iterera över tupel element, F = &&function()
// -lambda
template <typename Tuple, typename F,size_t... I> // size_t... I ; Compile time list från 0 -> I
void elementsInTuple(Tuple &&t, F &&f, index_sequence<I...>) {
  (f(get<I>(forward<Tuple>(t))), ...);
  // && ; referens till rvalue. Tillåter referens till
  // !temporärt! värde i scope utan att kopiera det.
  // Krävs då compile time object och värden är
  // temporära.
  // f() = funktion
  // get<I>(forward<Tuple>(t)) greppa element I från forward<Tuple>(t).
  // forward<>() ger && referens till ett värde. I detta fall Tupeln
  // ,... = fold expression. Kommer att kalla f() för varje element i Tupeln
  // Alltså: f(get<0>(t)), f(get<1>(t)), f(get<2>(t))... ger alltså alla
  // key:value i tupeln efter varann. Varje field i fields.
}

// Räknar du många gånger elementsInTuple kör och skapar lista med hjälp av
// make_index_sequence<>
template <typename Tuple, typename F> // F = &&function()
void forInTuple(Tuple &&t, F &&f) {
  constexpr auto size = tuple_size<remove_reference_t<Tuple>>::value;
  // Storleken av tupeln Tuple. remove_reference_t<> ger typen av
  // tupeln oavsett om den angess som värde, & eller &&
  elementsInTuple(forward<Tuple>(t), forward<F>(f),make_index_sequence<size>{});
  // kör elementsInTuple med Tupeln och
  // funktionen. I anges som
  // make_index_sequence<size>{}, alltså skapa
  // en lista med index från 0 -> size
  // index_sequenxe<I...> I elementsInTuple blir alltså:
  // index_sequence<make_index_sequence<size>{}> make_index_sequence<size>{}
  // evalueras till <0,...size-1> alltså lista av korrekt längd forward<F>(f)
  // ger &&function, alltså rvalue
}

// struct -> json genom att rekursera genom nested structs. Kan bara ta structs
// definierade i reflect<>
template <typename T> Json::Value structToJson(T &&obj) {
  using DecayedT = std::decay_t<T>;
  Json::Value json;

  if constexpr (reflectable<DecayedT>::value) {
    forInTuple(reflect<DecayedT>::fields, [&](const auto &field) {
      const auto &[name, ptr] = field;
      const auto &value = obj.*ptr;

      using FieldT = std::decay_t<decltype(value)>;

      // standard structs
      if constexpr (reflectable<FieldT>::value) {
        json[name] = structToJson(value);

        // vector
      } else if constexpr (is_specialization_of<FieldT, std::vector>::value) {
          Json::Value arr(Json::arrayValue);
          for (const auto &item : value) {
          arr.append(structToJson(item));
        }
        json[name] = arr;
      } else {
          json[name] = value;
      }
    });
  } else {
    static_assert(reflectable<DecayedT>::value,"Type not convertable. Cannot convert to JSON");
  }
  return json;
}

static string jsonToString(const Json::Value &json, bool pretty) {
  if (pretty) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "4";
    return Json::writeString(builder, json);
  } else {
      Json::StreamWriterBuilder builder;
      builder["indentation"] = "";
    return Json::writeString(builder, json);
  }
}

class Agent {
private:
  CurlPost crl;
  int maxTokens;
  size_t maxHistory;
  message systemMsg;
  string model;
  string toolChoice;
  // vector<Json::Value> tools;
  vector<message> history;

  // env & nät grejs
  string apiUrl;
  string token;

  message getResMessage(Json::Value res);
  void addHistory(message msg);
  void fetchConfig();

public:
  Agent(message sysMsg, bool useTools, string tool_choice = "none");
  ~Agent();

  Json::Value query(message msg);
};

#endif // !#ifndef AGENT_H
