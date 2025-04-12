#include "openai/agent.h"
#include <iostream>

using namespace std;

int main() {
  // System msg
  message sysMsg;
  sysMsg.role = "system";
  sysMsg.content = "Repeat my last message!";

  // Skapa agent
  Agent agent(sysMsg, false);

  while (true) {
    message usrMsg;
    usrMsg.role = "user";

    cout << "User: " << endl;
    cin >> usrMsg.content;
    cout << "Response: " << agent.query(usrMsg) << endl;
  }
  return 0;
};
