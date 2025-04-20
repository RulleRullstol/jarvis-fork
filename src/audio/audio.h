#ifndef AUDIO_H
#define AUDIO_H

#include <asio/io_context.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <asio.hpp>
#include "../utils/configHandler.h"
#include <string>


using namespace std;

class UDPHandler {
private:
  struct esp {
    string name;
    string addr;
    string port;
    bool alive;
    int lastKeepAlive;
  };
  // Alla våra fina esps
  vector<esp> esps;

  asio::io_context io_context;
  string broadcastAddr;
string broadcastPort;

string getBroadcast();
    
public:
  UDPHandler();
  ~UDPHandler();
  
};
#endif
