#include "audio.h"
#include <sstream>

vector<string> UDPHandler::charsToWords(string &chars) {
  vector<string> words;
  string word;
  istringstream iss(chars);
  while (iss >> word) {
    words.push_back(word);
  }
  return words;
}
