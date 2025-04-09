#include "webUtils.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
  WebUtils web = WebUtils();
  std::vector<std::string> headers;
  std::string res = web.send("google.com", headers, "");
  std::cout << "Respone: " << res << std::endl;

  return 0;
}
