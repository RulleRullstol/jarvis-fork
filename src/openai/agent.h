#ifndef AGENT_H
#define AGENT_H
#include "../utils/configHandler.h"
#include "../utils/env/envParser.h"
#include "../utils/web/webUtils.h"
#include <cstddef>
#include <cxxabi.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <memory>
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
      make_pair("tools", &T::tools), make_pair("tool_choice", &T::tool_choice),
      make_pair("messages", &T::messages));
};

template <typename T, typename = void> struct hasReflect : false_type {};

template <typename T>
struct hasReflect<T, std::void_t<decltype(reflect<std::decay_t<T>>::fields)>>
    : std::true_type {};
// definierad: decltype lyckas; -> true

template <typename T> struct reflectable {
  static constexpr bool value = hasReflect<T>::value;
};
// Default
template <typename T> struct is_vector : false_type {};

template <typename T, typename Alloc>
struct is_vector<vector<T, Alloc>> : true_type {
}; // True för typer av vector<> av alla typer med en allocator för att en
   // vector<> egentligen defineras som ex vector<int, allocator<int>> där
   // allocator<int> är instans av "Alloc" klassen

// compile time loop för att iterera över tupel element, F = &&function()
// -lambda
template <typename Tuple, typename F,
          size_t... I> // size_t... I ; Compile time list från 0 -> I
void elementsInTuple(Tuple &&t, F &&f,
                     index_sequence<I...>) { // Lambda, returnerar key/value
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
void forInTuple(Tuple &&t, F &&f) {   // Lambda, returnerar size
  constexpr auto size = tuple_size<remove_reference_t<Tuple>>::value;
  // Storleken av tupeln Tuple. remove_reference_t<> ger typen av
  // tupeln oavsett om den angess som värde, & eller &&
  elementsInTuple(forward<Tuple>(t), forward<F>(f),
                  make_index_sequence<size>{});
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

// Debug
template <typename T> void printType() {
  const char *mangled = typeid(std::decay_t<T>).name();
  int status = 0;
  std::unique_ptr<char[], void (*)(void *)> demangled(
      abi::__cxa_demangle(mangled, nullptr, nullptr, &status), std::free);

  std::cout << (status == 0 ? demangled.get() : mangled) << std::endl;
}

// Default  för structToJson för primitiva värden
// Kan inte ta vector<struct>, struct<vector<struct>> funkar
inline int recursionCounter = -1; // Debug
template <typename T> Json::Value structToJson(T &&obj) {
  using structType = decay_t<T>; // macro för T typ
  Json::Value json;
  recursionCounter++; // Debug

  if constexpr (reflectable<structType>::value) {
    forInTuple(
        reflect<structType>::fields,
        [&](const auto &field) { // & -> [capture] = scope
          const auto &[name, ptr] = field;
          const auto &value = obj.*ptr;

          using valType =
              decay_t<decltype(value)>; // För att undvika referenser till const
                                        // variable
          // cout << "Pair: " << name << " : " << " Type: ";
          // printType<decltype(value)>();

          // Om value är en till struct
          if constexpr (reflectable<valType>::value) {
            json[name] = structToJson(value);
            // cout << "Added: " << name << " = " << value << endl;

            // Om value är en vector
          } else if constexpr (is_vector<valType>::value) {
            Json::Value elements(Json::arrayValue);
            for (const auto &item : value) {
              using elementType =
                  decay_t<decltype(item)>; // Type av item, decay decltype av
                                           // anledningen att iteratorn är en
                                           // const referens till elementet
              if constexpr (reflectable<elementType>::value) {
                elements.append(structToJson(
                    item)); // Om structens element också är en struct.
                // cout << "Added: " << name << " Type: ";
                // printType<decltype(item)>();
              } else {
                elements.append(structToJson(item)); // Om inte
                // cout << "Added: " << name << " Type: ";
                // printType<decltype(item)>();
              }
            }
            json[name] =
                elements; // elements = vectorns innehåll som Json::Value
          } else {
            json[name] = value;
          }
        });
  } else {
    static_assert(reflectable<structType>::value,
                  "Type not reflectable. Cannot convert to JSON");
  }
  return json;
}

static string jsonToString(const Json::Value &json, bool pretty) {
  if (pretty) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = " ";
    return Json::writeString(builder, json);
  } else {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, json);
  }
}

class Agent {
private:
  int maxTokens;
  size_t maxHistory;
  message systemMsg;
  string model;
  string toolChoice;
  Json::Value tools;
  bool useTools;
  vector<message> history;

  // env & nät grejs
  string apiUrl;
  string token;

  void runToolCalls(Json::Value toolCalls);
  Json::Value resBodyToJson(string str);
  void addHistory(message msg);
  void fetchConfig();

public:
  Agent(message sysMsg, bool useTools, string tool_choice = "none");
  ~Agent();
  message getResMessage(const Json::Value &res);
  Json::Value query(message msg);
};

#endif // !#ifndef AGENT_H
