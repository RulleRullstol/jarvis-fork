
#include "../globals.h"
#include "configHandler.h"
#include <curl/curl.h>
#include <string>

using namespace std;
ConfigHandler mailConfig;

void sendManual() {
  string rec = mailConfig.getStandardEmail();
  string subj = "Jarvis Help";
  string path = "../res/jarvisLights.pdf";
  string body = "Hello " + mailConfig.getGender() + "!" +
                "\r\n\r\nI am here to help you understand me and how i "
                "work.\r\nI have attatched a pdf file containing the user "
                "manuel for my system. \r\n\r\nGood luck\r\n// Jarvis";

  globalCP.postMail(rec, subj, body, path);
}
