
#include "audio.h"
#include <asio/io_context.hpp>
#include <cstdint>

void UDPHandler::listenToESP(unsigned short port, deque<int16_t> &buffer) {
  asio::io_context ioContext;
  asio::ip::udp::socket socket(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
  array<char, 1024> recvBuffer;
  size_t sampleCount = 1024 / sizeof(int16_t);


  while (true) {
    asio::ip::udp::endpoint senderEndPoint;
    size_t bytesReceived = socket.receive_from(asio::buffer(recvBuffer), senderEndPoint);

    if (bytesReceived == 1024) {
      const int16_t* samples = reinterpret_cast<const int16_t*>(recvBuffer.data());

      buffer.insert(buffer.end(), samples, samples + sampleCount);
    }
  }
}
