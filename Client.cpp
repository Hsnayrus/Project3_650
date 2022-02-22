#include "Socket.hpp"

int main() {
  Socket Server("1235");
  Server.createSocket();
  Server.bindSocket();
  Server.listenOnSocket();
  Socket client("127.0.0.1", "4445");
  std::string messageToSend = "\nRitvizzzzzzz\n";
  std::vector<char> newVector(messageToSend.begin(), messageToSend.end());
  std::cout << "Clients message is :" << messageToSend
            << "and newVector: " << newVector.size() << std::endl;
  client.createSocket();
  client.sendToServer(newVector);
  std::vector<char> temp = client.readBuffer(client.getSocket_FD());
  std::string message(temp.begin(), temp.end());
  std::cout << "$$$$" << message << std::endl;
  while (true) {
    Server.acceptConnections(100);
  }
}