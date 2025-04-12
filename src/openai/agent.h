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

template <typename T> using hasReflect = decltype(reflect<T>::fields, void());
// Testa läsa typen på reflect::field. Om
// definierad: decltype lyckas; -> void
// Annars

template <typename T> struct reflectable {
  static constexpr bool value =
      is_same_v<hasReflect<T>,
                void>; // Kolla om hasReflect ger void isf true. Om decltype
                       // misslyckas kommer is_same ge false.
};

// compile time loop för att iterera över tupel element, F = &&function()
// -lambda
template <typename Tuple, typename F,
          size_t... I> // size_t... I ; Compile time list från 0 -> I
void elementsInTuple(
    Tuple &&t, F &&f,
    index_sequence<I...>) { // && ; referens till rvalue. Tillåter referens till
                            // !temporärt! värde i scope utan att kopiera det.
                            // Krävs då compile time object och värden är
                            // temporära.
  (f(get<I>(forward<Tuple>(t))), ...); // snusk
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
  constexpr auto size = tuple_size<remove_reference_t<Tuple>>::
      value; // Storleken av tupeln Tuple. remove_reference_t<> ger typen av
             // tupeln oavsett om den angess som värde, & eller &&
  elementsInTuple(
      forward<Tuple>(t), forward<F>(f),
      make_index_sequence<size>{}); // kör elementsInTuple med Tupeln och
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
template <typename T> Json::Value structToJson(T &obj) {
  Json::Value json;

  if constexpr (reflectable<T>::value) { // Kolla om struct T är definierad i
                                         // reflect<>
    forInTuple(
        reflect<T>::fields, // Felar här
        [&](const auto
                &field) { // Loopa över reflect structens field (tupel). Denna
                          // {} är F = && function() med en lambda funktion
          const auto &[name, ptr] = field;
          const auto &value = obj.*ptr;

          if constexpr (reflectable<std::decay_t<decltype(value)>>::
                            value) // Om value är definerad i reflect<> så kalla
                                   // funktionen rekursivt
            json[name] = structToJson(
                value); // Kör funktionen för value och sätt in det i json
          else
            json[name] = value;
        });
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
  size_t maxHistory;
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
