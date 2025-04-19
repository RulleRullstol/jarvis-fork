#include "globals.h"
#include "openai/agent.h"
#include "utils/lightHandler.h"
#include <iostream>
#include <string>

using namespace std;

CurlPost globalCP;

int main() {
  // System msg
  message sysMsg;
  sysMsg.role = "system";
  sysMsg.content = "You are Jarivs a helpful assistant. Answer in short "
                   "answers and refer to the user as Sir!";

  // Skapa agent
  Agent agent(sysMsg, true);

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
