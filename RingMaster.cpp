/*
This code was made after reading the following article:

https://beej.us/guide/bgnet/html//index.html


I have also taken references from the 07-tcp_example.zip file available
in the Resources tab in Sakai
*/

#include "Socket.hpp"

void parsePairs(std::vector<std::pair<int, std::string> > & clients) {
  std::cout << "Connections uptil now: \n";
  for (size_t i = 0; i < clients.size(); i++) {
    std::cout << "Client IP: " << clients[i].second << " client_fd " << clients[i].first
              << std::endl;
  }
}

int main() {
  Socket s1("4445");
  s1.createSocket();
  s1.bindSocket();
  s1.listenOnSocket();
  std::vector<std::pair<int, std::string> > clients;
  for (int i = 0; i < 2; i++) {
    std::pair<int, std::string> temp = s1.acceptConnections(2);
    clients.push_back(temp);
  }
  std::cout << "End of Loop\n";
  std::string message = "Hello hello awaaz aa rahi hai?";
  s1.sendToClient(clients[0].first, message);
}