#include "Socket.hpp"

int main() {
  Socket client("127.0.0.1", "4445");
  std::string messageToSend = "Hello darkness";
  std::vector<char> newVector(messageToSend.begin(), messageToSend.end());
  std::cout << "Clients message is :" << messageToSend.size()
            << "and newVector: " << newVector.size() << std::endl;
  client.createSocket();
  client.sendToServer(newVector);
}