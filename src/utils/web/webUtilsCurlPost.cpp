#include "webUtils.h"
#include <iostream>

void CurlPost::setHeaders(CURL *curl, const vector<string> &headers) {
  struct curl_slist *chunk = NULL;
  for (const auto &header : headers)
    chunk = curl_slist_append(chunk, header.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
}

// Set post/put body
void CurlPost::setBody(CURL *curl, const string &body) {
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
}

// callback function för att skriva till response. Spännande grunka
size_t CurlPost::writeCallback(void *contents, size_t size, size_t nmemb, string *userp) { // userp är ptr till reponse
  size_t totalSize = size * nmemb;
  string *response = reinterpret_cast<string *>(userp); // userp->reponse
  response->append((char *)contents, totalSize);
  return totalSize;
}

CurlPost::CurlPost() { // init libcurl
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

// Clean up :)
CurlPost::~CurlPost() { curl_global_cleanup(); }

// Send POST/PUT. returns std::string response
string CurlPost::post(const string &url, const vector<string> &headers, const string &body) {
  CURL *curl = curl_easy_init();

  string response;

  if (!curl)
    throw runtime_error("CURL is not initialized"); // I fall man missat
  response.clear();
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  setHeaders(curl, headers);
  setBody(curl, body);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback); // Callback för server svar
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // Response -> userp

  // Skicka request
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    curl_easy_cleanup(curl);
    throw runtime_error("curl request failed: " + string(curl_easy_strerror(res)));
  }

  curl_easy_cleanup(curl);
  return response;
}

void CurlPost::postMail(const string &rec, const string &subj,
                        const string &body, string &pdfPath) {
  string sender = "jarvis.mailservice@gmail.com";

  CURL *curl = curl_easy_init();

  struct curl_slist *recievers = nullptr;

  curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
  curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
  curl_easy_setopt(curl, CURLOPT_USERNAME, sender.c_str());
  curl_easy_setopt(curl, CURLOPT_PASSWORD, curlCH.getEmailPswd().c_str());
  curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + sender + ">").c_str());

  recievers = curl_slist_append(nullptr, ("<" + rec + ">").c_str());
  curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recievers);

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, ("Subject: " + subj).c_str());
  headers = curl_slist_append(headers, ("To: " + rec).c_str());
  headers = curl_slist_append(headers, ("From: Jarvis"));

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  curl_mime *mime = curl_mime_init(curl);

  curl_mimepart *part = curl_mime_addpart(mime);
  curl_mime_data(part, body.c_str(), CURL_ZERO_TERMINATED);
  curl_mime_type(part, "text/plain");

  if (pdfPath != "") {
    part = curl_mime_addpart(mime);
    curl_mime_filedata(part, pdfPath.c_str());
    curl_mime_filename(part, "attachment.pdf");
    curl_mime_type(part, "application/pdf");
    curl_mime_encoder(part, "base64");
  }

  curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

  CURLcode r = curl_easy_perform(curl);

  if (r != CURLE_OK)
    cerr << "Curl error: " << curl_easy_strerror(r) << "\n";
  else
    cout << "Email sent with attachment.\n";

  curl_mime_free(mime);
  curl_slist_free_all(recievers);
  curl_easy_cleanup(curl);
}
