#include "audio.h"

UDPHandler::UDPHandler() {
  ConfigHandler conf;
  broadcastAddr = getBroadcast();
  broadcastPort = conf.getValue("esp_general", "broadcastPort");
  // Populate esps vector
  int targetEsps = stoi(conf.getValue("esp_general", "count"));
  int maxRetries = targetEsps * stoi(conf.getValue("esp_general", "max_retries"));
  int retries = 0;
  string msgTemplate = conf.getValue("esp_general", "broadcast_msg");

  asio::ip::udp::resolver resolver(io_context);
  asio::ip::udp::endpoint broadCastEndpoint = *resolver.resolve(asio::ip::udp::v4(), broadcastAddr, broadcastPort);
  asio::ip::udp::socket socket(io_context);
  array<char, 1024> recvBuffer; // 1024 is enough for ack 

  while (esps.size() != targetEsps) {
    // Listen for esps
    size_t len = socket.receive_from(asio::buffer(recvBuffer), broadCastEndpoint);
    string recvMsg = string(recvBuffer.data(), len);
  
    // Ta bort msgTemplate
    int pos = recvMsg.find(msgTemplate);
    if (pos != string::npos) {
      recvMsg.erase(pos, msgTemplate.length());
      int dividerPos = recvMsg.find(" "); // Hitta pos av divider mellan namn och port
      
      // Skapa esp struct
      if (dividerPos != string::npos) { // Kan bli problem om tidigare erase 
        esp foundESP;
        foundESP.name = recvMsg.substr(1, dividerPos); // Räkna med space mellan ord
        foundESP.port = recvMsg.substr(dividerPos, recvMsg.size());
        foundESP.addr = broadCastEndpoint.address().to_string();
        esps.push_back(foundESP);
      } else {
        cout << "Recieved malformed response from ESP, retrying..." << endl;
      }
    }

    if (retries >= maxRetries && esps.size() != 0) {
      cerr << "Max retries reached looking for ESPS on: " << broadcastAddr << ":" << broadcastPort << endl;
      break;
    }
  }
}
