#include "Socket.hpp"

int main() {
  Socket player("127.0.0.1", "4445");
  std::string messageToSend = "Ready for connection.";
  std::vector<char> newVector(messageToSend.begin(), messageToSend.end());
  player.createSocket();
  player.sendToServer(newVector);
  newVector.clear();
  player.receivePotato(player.getSocket_FD());
  //   newVector = player.readBuffer(player.getSocket_FD());
  //   std::string clientIP(newVector.begin(), newVector.end());
  //   std::cout << "client IP" << clientIP << std::endl;
  //   if (clientIP.size() != 0) {
  //     size_t middle = clientIP.find("\n");
  //     std::string ipAddr = clientIP.substr(0, middle);
  //     std::string port = clientIP.substr(middle, clientIP.size() - middle);
  //     Socket client(ipAddr, port);
  //     client.createSocket();
  //     newVector.clear();
  //     std::string newS = "Jamba Juice";
  //     newVector = std::vector<char>(newS.begin(), newS.end());
  //     client.sendToServer(newVector);
  //   }
}