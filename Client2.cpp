#include "Socket.hpp"

int main() {
  Socket player("127.0.0.1", "4445");
  std::string messageToSend = "Client 2 sending message";
  std::vector<char> newVector(messageToSend.begin(), messageToSend.end());
  player.createSocket();
  player.sendToServer(newVector);
  newVector.clear();
  newVector = player.readBuffer(player.getSocket_FD());
  std::string clientIP(newVector.begin(), newVector.end());
  std::cout << "client IP" << clientIP << std::endl;
  if (clientIP.size() != 0) {
    std::cout << "Coming in this loop\n";
    Socket client(clientIP.c_str(), "1235");
    std::cout << "Connecting to(2) : ", clientIP.c_str();
    client.createSocket();
    std::cout << "Socket Created\n";
    newVector.clear();
    std::string newS = "Jamba Juice";
    newVector = std::vector<char>(newS.begin(), newS.end());
    client.sendToServer(newVector);
  }
}