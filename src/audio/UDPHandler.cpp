#include "audio.h"
#include <algorithm>

UDPHandler::UDPHandler() {
  ConfigHandler conf;
  broadcastAddr = getBroadcast();
  broadcastPort = conf.getValue("esp_general", "broadcastPort");
  // Populate esps vector
  int targetEsps = stoi(conf.getValue("esp_general", "count"));
  int maxRetries = targetEsps * stoi(conf.getValue("esp_general", "max_retries"));
  int retries = 0;
  string msgTemplate = conf.getValue("esp_general", "broadcast_msg");
  string ackTemplate = conf.getValue("esp_general", "ack_msg");

  asio::ip::udp::resolver resolver(io_context);
  // Sending endpoint
  asio::ip::udp::endpoint broadCastEndpoint = *resolver.resolve(asio::ip::udp::v4(), broadcastAddr, broadcastPort);
  // Recieving endpoint
  asio::ip::udp::endpoint recvEndpoint;

  asio::ip::udp::socket socket(io_context);
  array<char, 256> recvBuffer; // 1024 is enough for ack 

  while (esps.size() != targetEsps) {
    // Listen for esps
    size_t len = socket.receive_from(asio::buffer(recvBuffer), recvEndpoint);
    string recvMsg = string(recvBuffer.data(), len);
  
    // Ta bort msgTemplate och rengöra 
    int pos = recvMsg.find(msgTemplate);
    if (pos != string::npos) {
      recvMsg.erase(pos, msgTemplate.length());

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
      
      // Skapa esp struct
      int dividerPos = recvMsg.find(" "); // Hitta pos av divider mellan namn och port
      if (dividerPos != string::npos) {
        esp foundESP;
        foundESP.name = recvMsg.substr(1, dividerPos); // Räkna med space mellan ord
        foundESP.endpoint = recvEndpoint;
        esps.push_back(foundESP);
        cout << "Found ESP: " << foundESP.name << endl;

        // Skapa ack msg och skicka till esp
        array<char, 256> ackMsg;
        string tempStr = ackTemplate + " " + foundESP.name;
        copy_n(tempStr.begin(), min(tempStr.size(), ackMsg.size() - 1), ackMsg.begin()); // in med skiten i array
        ackMsg[min(tempStr.size(), ackMsg.size() -1)] = '\0'; // Null termiantor
        

        socket.send_to(asio::buffer(ackMsg.data(), strlen(ackMsg.data())), recvEndpoint);
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

UDPHandler::~UDPHandler() {return;}
