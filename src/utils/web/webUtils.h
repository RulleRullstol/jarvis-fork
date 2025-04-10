
// sudo apt-get install libcurl4-openssl-dev

#ifndef WEBUTILS_H
#define WEBUTILS_H

#include <cstddef>
#include <curl/curl.h>
#include <stdexcept>
#include <string>
#include <vector>

class CurlPost {
private:
  std::string response;
  CURL *curl;

  void setHeaders(const std::vector<std::string> &headers);
  void setBody(const std::string &body);

  static size_t writeCallback(void *contents, size_t size, size_t nmemb,
                              std::string *userp);

public:
  CurlPost();
  ~CurlPost();

  std::string post(const std::string &url,
                   const std::vector<std::string> &headers,
                   const std::string &body);
};

#endif
