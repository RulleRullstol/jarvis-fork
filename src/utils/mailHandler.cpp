

#include "configHandler.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

using namespace std;
ConfigHandler ch = ConfigHandler();

void sendMail(const string &rec, const string &subj, const string &body,
              string &pdfPath) {

  string sender = "jarvis.mailservice@gmail.com";

  CURL *curl = curl_easy_init();

  struct curl_slist *recievers = nullptr;

  curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
  curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
  curl_easy_setopt(curl, CURLOPT_USERNAME, sender.c_str());
  curl_easy_setopt(curl, CURLOPT_PASSWORD, ch.getEmailPswd().c_str());
  curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + sender + ">").c_str());

  recievers = curl_slist_append(nullptr, ("<" + rec + ">").c_str());
  curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recievers);

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

void sendManual() {
  string res = ch.getStandardEmail();
  string subj = "Jarvis Help";
  string path = "../../res/jarvisLights.pdf";
  string body = "Hello " + ch.getGender() +
                "\r\n\r\nI am here to help you understand me and how i "
                "work.\r\nI have attatched a pdf file containing the user "
                "manuel for my system. \r\n\r\nGood luck\r\n// Jarvis";

  sendMail(res, subj, body, path);
}
