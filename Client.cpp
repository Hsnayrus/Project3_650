#include "Socket.hpp"
#include "functions.hpp"

// class Client {
//   Socket ServerForPlayer;
//   Socket ClientForPlayer;
//   Socket ClientForRingMaster;

//  public:
//   Client(std::string ringMasterIP, std::string ringMasterPort) :
//       ClientForRingMaster(ringMasterIP, ringMasterPort) {}

//   void createServerSocket(std::string portNum) {
//     ServerForPlayer = Socket("0.0.0.0", "portNum");
//     ServerForPlayer.createSocket();
//     ServerForPlayer.bindSocket();
//     ServerForPlayer.listenOnSocket();
//   }

//   void createClientSocket(std::string hostName, std::string port) {
//     ClientForPlayer = Socket(hostName.c_str(), port.c_str());
//   }

//   void sendToPlayerServer(std::string message) {
//     std::vector<char> buffer(message.begin(), message.end());
//     ServerForPlayer.sendToServer(buffer);
//   }

//   void sendToRingMaster(std::string message) {
//     std::vector<char> buffer(message.begin(), message.end());
//     ClientForRingMaster.sendToServer(buffer);
//   }

//   void continueGame() {
//     fd_set newSet;
//     std::pair<int, std::string> temp = ServerForPlayer.acceptConnections();
//     int playerClient_fd = ClientForPlayer.getSocket_FD();
//     int playerServer_fd = temp.first;
//     int playerRingMaster_fd = ClientForRingMaster.getSocket_FD();

//     int fdmax = (playerClient_fd > playerServer_fd)
//                     ? ((playerClient_fd > playerRingMaster_fd) ? playerClient_fd
//                                                                : playerRingMaster_fd)
//                     : ((playerServer_fd > playerRingMaster_fd) ? playerServer_fd
//                                                                : playerRingMaster_fd);

//     while (true) {
//       FD_ZERO(&newSet);
//       FD_SET(temp.first, &newSet);
//       FD_SET(ClientForPlayer.getSocket_FD(), &newSet);
//       FD_SET(ClientForRingMaster.getSocket_FD(), &newSet);
//       select(fdmax + 1, &newSet, NULL, NULL, NULL);
//       if (FD_ISSET(playerClient_fd, &newSet)) {
//         std::cout << "Received message from left player";
//       }
//       else if (FD_ISSET(playerRingMaster_fd, &newSet)) {
//         std::cout << "Received from ringmaster\n";
//       }
//       else if (FD_ISSET(playerServer_fd, &newSet)) {
//         std::cout << "Received from right player\n";
//       }
//     }
//   }
// };

std::pair<int, int> parseIDNoPlayers(std::string message) {
  size_t start = message.find("//////");
  int id = str2num(message.substr(0, start));
  int noPlayers = str2num(message.substr(start + 6));
  return std::make_pair(id, noPlayers);
}

std::pair<std::string, std::string> parseIPPort(std::string message) {
  size_t start = message.find("//////");
  std::string ip = message.substr(0, start);
  std::cout << "==========================================\n";
  std::cout << message << message.size() << std::endl;

  std::cout << "==========================================\n";
  std::string port = message.substr(start + 6);
  return std::make_pair(ip, port);
}

int main() {
  Socket client("127.0.0.1", "4448");
  std::string messageToSend = "Ready for connection.";
  std::vector<char> newVector(messageToSend.begin(),
                              messageToSend.begin() + messageToSend.size());
  client.createSocket();
  client.sendToServer(newVector);

  newVector.clear();
  cInfo_t info;
  client.receiveClientInfo(client.getSocket_FD(), info);

  // int len = recv(client.getSocket_FD(), buffer.data(), buffer.size(), 0);
  // std::string ipPort(buffer.begin(), buffer.begin() + len);
  // std::pair<std::string, std::string> ipPortPair = parseIPPort(ipPort);
  // std::cout << ipPortPair.first << "______________________" << ipPortPair.second;
  // //Listener in every client
  // Socket rightClient(ipPortPair.first.c_str(), ipPortPair.second.c_str());
  // rightClient.createSocket();
  // std::string ipMessage(buffer.begin(), buffer.end());
  // std::cout << "$$$$" << ipMessage << std::endl;
  return 0;
}