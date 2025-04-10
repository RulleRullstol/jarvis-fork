#include "webUtils.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
  CurlPost web;
  std::vector<std::string> headers;
  std::string res = web.post("google.com", headers, "");
  std::cout << "Respone: " << res << std::endl;

  return 0;
}
