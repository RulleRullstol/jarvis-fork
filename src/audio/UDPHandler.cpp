#include "audio.h"
#include <asio/socket_base.hpp>

UDPHandler::UDPHandler() {
  ConfigHandler conf;
  // Populate esps vector
  int broadcastPort = stoi(conf.getValue("esp_general", "broadcast_port"));
  int targetEsps = stoi(conf.getValue("esp_general", "count"));
  int maxRetries = targetEsps * stoi(conf.getValue("esp_general", "max_retries"));
  int retries = 0;
  string msgIdentifer = conf.getValue("esp_general", "broadcast_msg");
  string ackTemplate = conf.getValue("esp_general", "ack_msg");

  asio::ip::udp::resolver resolver(io_context);
  // Recieving endpoint
  asio::ip::udp::endpoint recvEndpoint;
  
  // Setup socket for recv. Listen to all ports on all interfaces
  asio::ip::udp::socket socket(io_context);
  socket.open(asio::ip::udp::v4());
  socket.set_option(asio::socket_base::broadcast(true)); // allow broadcast
  socket.set_option(asio::socket_base::reuse_address(true));
  socket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), broadcastPort)); // Bind to all ports on all interfaces

  array<char, 256> recvBuffer; // 1024 is enough for ack 
  
  cout << "Listening for ESPs..." << endl;
  while (esps.size() != targetEsps) {
    // Listen for esps
    size_t len = socket.receive_from(asio::buffer(recvBuffer), recvEndpoint);
    cout << "Packet recieved, parsing frame..." << endl;
    string rcvdMsg = string(recvBuffer.data(), len);
    vector<string> rcvdWords = charsToWords(rcvdMsg);
  
    // Parse msg
    if (rcvdWords[0] == msgIdentifer) {
      esp esp;
      esp.name = rcvdWords[1];
      esp.endpoint = asio::ip::udp::endpoint(asio::ip::make_address(recvEndpoint.address().to_string()), stoi(rcvdWords[2]));
      esp.keepalivePort = stoi(rcvdWords[3]);
      cout << "Found ESP: " << esp.name << " at: " << esp.endpoint.address() << endl;
      
      // Handshake & stoppa in i vector om ESP svarar & vi inte redan pratar med espn
      keepAlive(esp);
      if (esp.status == "alive" && [&]() {
          for (auto& savedEsp : esps) {
            if (savedEsp.name == esp.name) {
                  cout << "Error: ESP: " << esp.name << " already parsed..." << endl;
                  return false;
            }
          }
        return true;
      }()) {
        cout << "Keepalive handshake successfull." << endl;
        esps.push_back(esp);
      }
    } else {
      cout << "Packet missing identifier." << endl;
    }

    if (retries >= maxRetries && esps.size() != 0) {
      cerr << "Max retries reached looking for ESPS on: " << broadcastAddr << ":" << broadcastPort << endl;
      break;
    }
  }
  cout << "Handshake completed with: " << esps.size() << "/" << targetEsps << " ESPs." << endl;
}

UDPHandler::~UDPHandler() {return;}
