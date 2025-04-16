#include "openai/agent.h"
#include "utils/lightHandler.h"
#include <array>
#include <iostream>
#include <string>

using namespace std;

int main() {
  // System msg
  message sysMsg;
  sysMsg.role = "system";
  sysMsg.content =
      "You are Jarivs a helpful assistant. Refer to the user as Sir!";
  cout << "test" << endl;
  LightRequest l;
  l.brightness = 255;
  l.state = true;

  cout << "test2" << endl;
  array<int, 3> rgb = {255, 0, 0};

  l.rgb = rgb;

  l.id = "light.hall";
  cout << "hej" << endl;
  setLights(l);

  // Skapa agent
  Agent agent(sysMsg, false);

  while (true) {
    string input;
    message usrMsg;
    usrMsg.role = "user";

    cout << "User: ";
    getline(cin, input);
    usrMsg.content = input;

    Json::Value res = agent.query(usrMsg); // Only one call

    // Optional: handle or print response
    string reply = agent.getResMessage(res).content;
    cout << "Assistant: " << reply << endl;

    // Add a break condition if you want to stop the loop, like:
    if (input == "exit")
      break;
  }

  return 0;
}
