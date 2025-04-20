#ifndef AUDIO_H
#define AUDIO_H

#include <asio/io_context.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <array>
#include <deque>
#include <asio.hpp>
#include <cstdint>
#include "../utils/configHandler.h"
#include <string>


using namespace std;

short misistt(vector<deque<int16_t>> &buffers);

class UDPHandler {
private:
  struct esp {
    string name;
    asio::ip::udp::endpoint endpoint;
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
  void listenToESP(unsigned short port, deque<int16_t> &buffer); 
};
#endif
