#include "audio.h"
#include <algorithm>
#include <asio/socket_base.hpp>

UDPHandler::UDPHandler() {
  ConfigHandler conf;
  // Populate esps vector
  int broadcastPort = stoi(conf.getValue("esp_general", "broadcast_port"));
  int targetEsps = stoi(conf.getValue("esp_general", "count"));
  int maxRetries = targetEsps * stoi(conf.getValue("esp_general", "max_retries"));
  int retries = 0;
  string msgTemplate = conf.getValue("esp_general", "broadcast_msg");
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
    cout << "Packet recieved, parsing..." << endl;
    string recvMsg = string(recvBuffer.data(), len);
  
    // Ta bort msgTemplate och rengöra 
    int pos = recvMsg.find(msgTemplate);
    if (pos != string::npos) {
      recvMsg.erase(pos, (msgTemplate.length()));

      // Hitta spaces och \0 runt en string och göra en substring utan dem 
      int start = 0;
      int end = recvMsg.size();
      while (start < end && (isspace(static_cast<unsigned char>(recvMsg[start])) || recvMsg[start] == '\0')) {
        start++;
      }
      while (end > start && (isspace(static_cast<unsigned char>(recvMsg[end - 1])) || recvMsg[end - 1] == '\0')) {
        end--;
      }
      recvMsg = recvMsg.substr(start, end - start);
      
      // Populera esp struct om vi hittat esp
      int dividerPos = recvMsg.find(" "); // Hitta pos av divider mellan namn och port
      if (dividerPos != string::npos) {
        esp foundESP;
        foundESP.name = recvMsg.substr(0, dividerPos); // Räkna med space mellan ord
        foundESP.endpoint = recvEndpoint;

        // Stoppa in om inte finns
        if ([&]() {
          for (const auto& esp : esps) {
            if (esp.name == foundESP.name)
              return true;
          }
          return false;
        }()) {
          esps.push_back(foundESP);
        }  

        cout << "Found: " << foundESP.name << " at: " << recvEndpoint.address() << endl;

        // Skapa ack msg och skicka till esp
        array<char, 256> ackMsg;
        string tempStr = ackTemplate + " " + foundESP.name;
        copy_n(tempStr.begin(), min(tempStr.size(), ackMsg.size() - 1), ackMsg.begin()); // in med skiten i array
        socket.send_to(asio::buffer(ackMsg.data(), strlen(ackMsg.data())), recvEndpoint);
        cout << "ACK sent to: " << foundESP.name << " at: " << recvEndpoint.address() << ":" << recvEndpoint.port() << endl;
        retries++; // Increment retries
      } else {
        cout << "Recieved malformed response from ESP, retrying..." << endl;
      }
    }
    cout << "Packet not from esp";

    if (retries >= maxRetries && esps.size() != 0) {
      cerr << "Max retries reached looking for ESPS on: " << broadcastAddr << ":" << broadcastPort << endl;
      break;
    }
  }
}

UDPHandler::~UDPHandler() {return;}
