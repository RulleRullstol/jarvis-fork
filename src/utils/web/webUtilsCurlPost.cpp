#include "webUtils.h"

void CurlPost::setHeaders(const vector<string> &headers) {
  struct curl_slist *chunk = NULL;
  for (const auto &header : headers)
    chunk = curl_slist_append(chunk, header.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
}

// Set post/put body
void CurlPost::setBody(const string &body) {
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
}

// callback function för att skriva till response. Spännande grunka
size_t CurlPost::writeCallback(void *contents, size_t size, size_t nmemb,
                               string *userp) { // userp är ptr till reponse
  size_t totalSize = size * nmemb;
  string *response = reinterpret_cast<string *>(userp); // userp->reponse
  response->append((char *)contents, totalSize);
  return totalSize;
}

CurlPost::CurlPost() { // init libcurl
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if (!curl)
    throw runtime_error("Failed to init libcurl");
}

// Clean up :)
CurlPost::~CurlPost() {
  if (curl) {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }
}

// Send POST/PUT. returns std::string response
string CurlPost::post(const string &url, const vector<string> &headers,
                      const string &body) {
  if (!curl)
    throw runtime_error("CURL is not initialized"); // I fall man missat
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  setHeaders(headers);
  setBody(body);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                   writeCallback); // Callback för server svar
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // Response -> userp

  // Skicka request
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    throw runtime_error("curl request failed: " +
                        string(curl_easy_strerror(res)));
  return response;
}
