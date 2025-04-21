#include "audio.h"
#include <asio/io_context.hpp>
#include <asio/socket_base.hpp>
#include "../globals.h"
#include <asio/steady_timer.hpp>
#include <chrono>

// Sends a keepalive msg to the ESP and updates last_keepalive 
// Returns True if ESP acknowledges request.
// Return False if ESP does not acknowledge.
void UDPHandler::keepAlive(esp &esp) {
  string keepaliveMsg = globalCH.getValue("esp_general", "keepalive_msg");
  string ackMsg = globalCH.getValue("esp_general", "ack_msg");
  string keepaliveInterval = globalCH.getValue("esp_general", "keepalive_interval");
  int maxRetries = stoi(globalCH.getValue("esp_general", "max_retries"));
  int retries = 0;
  
  asio::io_context ioctx;
  asio::steady_timer timer(ioctx);
  globalCH.getValue("esp_general", "keepalive_msg");
  asio::ip::udp::socket socket(ioctx);
  socket.open(asio::ip::udp::v4());
  socket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), esp.endpoint.port())); // TODO: keepalive meddelanden måste ske på sin egen port, annars ajjabajja
  
  // keepalive ESP_0 10 meddelande
  string msg = keepaliveMsg + " " + esp.name + " " + keepaliveInterval;
  vector<string> msgWords;
  string word;
  istringstream iss(msg);
  while (iss >> word) {
    msgWords.push_back(word);
  }

  array<char, 256> buffer;
  copy_n(msg.begin(), min(msg.size(), ackMsg.size() - 1), msg.begin());

  // Send keepalive to ESP
  bool ack = false; // Om vi fått tbks en ack
  while (maxRetries >= retries  && !ack) {
    cout << "Sentkeepalive frame to: " << esp.name << endl;
    socket.send_to(asio::buffer(buffer.data(), strlen(buffer.data())), esp.endpoint);

    // Wait for response
    cout << "Awaiting keepalive ACK from: " << esp.name << endl;
    array<char, 1024> recvBuffer;

    // Start recive
    socket.async_receive_from(asio::buffer(recvBuffer), esp.endpoint, [&](error_code ec, size_t bytesRecvd) {
      if (!ec) {
        // Kolla om svar korrekt
        string recvMsg = string(recvBuffer.data(), bytesRecvd);
        // Gör om rcvdMsg till vector av ord
        vector<string> rcvdWords;
        string word;
        istringstream iss(recvMsg);
        while (iss >> word) {
          rcvdWords.push_back(word);
        }
        // Jämför
        if (rcvdWords == msgWords) {
          cout << "Recieved keepalive ACK from: " << esp.name << endl;
          esp.keepaliveInterval = stoi(keepaliveInterval);
          esp.keepaliveLast = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
          esp.status = "alive";
        } 
      } else {
        cout << "keepalive recieve error: " << ec.message() << endl;
      }
    });

    //timeout timer - timeout efter keepalive interval
    timer.expires_after(chrono::seconds(stoi(keepaliveInterval)));
    timer.async_wait([&](error_code ec) {
      if (!ec) {
        cout << "keepalive recieve timeout, retrying..." << endl;
        socket.cancel();
      }
    });

    ioctx.run(); // Start sync funktioner. Blockar tills de är färdiga
    ioctx.reset(); // Preppa för att köra sync func igen
    retries++;
  }
  // Om vi inte fick svar
  if (!ack) {
    esp.status = "timeout";
  }
  return;
}
