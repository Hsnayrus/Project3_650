#include "Socket.hpp"

class Client {
  Socket ServerForPlayer;
  Socket ClientForPlayer;
  Socket ClientForRingMaster;

 public:
  Client(std::string ringMasterIP, std::string ringMasterPort) :
      ClientForRingMaster(ringMasterIP, ringMasterPort) {}

  void createServerSocket(std::string portNum) {
    ServerForPlayer = Socket("0.0.0.0", "portNum");
    ServerForPlayer.createSocket();
    ServerForPlayer.bindSocket();
    ServerForPlayer.listenOnSocket();
  }

  void createClientSocket(std::string hostName, std::string port) {
    ClientForPlayer = Socket(hostName.c_str(), port.c_str());
  }

  void sendToPlayerServer(std::string message) {
    std::vector<char> buffer(message.begin(), message.end());
    ServerForPlayer.sendToServer(buffer);
  }

  void sendToRingMaster(std::string message) {
    std::vector<char> buffer(message.begin(), message.end());
    ClientForRingMaster.sendToServer(buffer);
  }

  void continueGame() {
    fd_set newSet;
    std::pair<int, std::string> temp = ServerForPlayer.acceptConnections();
    int playerClient_fd = ClientForPlayer.getSocket_FD();
    int playerServer_fd = temp.first;
    int playerRingMaster_fd = ClientForRingMaster.getSocket_FD();

    int fdmax = (playerClient_fd > playerServer_fd)
                    ? ((playerClient_fd > playerRingMaster_fd) ? playerClient_fd
                                                               : playerRingMaster_fd)
                    : ((playerServer_fd > playerRingMaster_fd) ? playerServer_fd
                                                               : playerRingMaster_fd);

    while (true) {
      FD_ZERO(&newSet);
      FD_SET(temp.first, &newSet);
      FD_SET(ClientForPlayer.getSocket_FD(), &newSet);
      FD_SET(ClientForRingMaster.getSocket_FD(), &newSet);
      select(fdmax + 1, &newSet, NULL, NULL, NULL);
      if (FD_ISSET(playerClient_fd, &newSet)) {
        std::cout << "Received message from left player";
      }
      else if (FD_ISSET(playerRingMaster_fd, &newSet)) {
        std::cout << "Received from ringmaster\n";
      }
      else if (FD_ISSET(playerServer_fd, &newSet)) {
        std::cout << "Received from right player\n";
      }
    }
  }
};

int main() {
  Socket client("127.0.0.1", "4445");
  std::string messageToSend = "1235";
  std::vector<char> newVector(messageToSend.begin(), messageToSend.end());
  std::cout << "Clients message is :" << messageToSend
            << "and newVector: " << newVector.size() << std::endl;
  client.createSocket();
  client.sendToServer(newVector);

  Socket Server("1235");
  Server.createSocket();
  Server.bindSocket();
  Server.listenOnSocket();
  while (true) {
    std::pair<int, std::string> accepted = Server.acceptConnections();
    std::vector<char> temp = Server.readBuffer(accepted.first);
    std::string message(temp.begin(), temp.end());
    std::cout << "$$$$" << message << std::endl;
  }
}