#ifndef ENVPARSER_H
#define ENVPARSER_H

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

string getEnvValue(const string &key, const string &envFilePath = ".env");

#endif // ENVPARSER_H
