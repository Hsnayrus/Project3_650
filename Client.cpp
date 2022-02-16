#include "Socket.hpp"

int main() {
  Socket client("127.0.0.1", "4445");
  std::string messageToSend = "Hello World from another terminal";
  std::vector<char> newVector(messageToSend.begin(), messageToSend.end());
  client.createSocket();
  client.sendToServer(newVector);
}