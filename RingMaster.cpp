/*
This code was made after reading the following article:

https://beej.us/guide/bgnet/html//index.html


I have also taken references from the 07-tcp_example.zip file available
in the Resources tab in Sakai
*/

#include <cassert>

#include "Socket.hpp"
class RingMaster {
  Socket Server;
  std::vector<std::pair<int, std::pair<std::string, std::string> > > players;
  size_t noPlayers;

 public:
  RingMaster() : Server(Socket("4445")) {
    players = std::vector<std::pair<int, std::pair<std::string, std::string> > >();
    noPlayers = 0;
    makeServerSocket();
  }

  RingMaster(size_t playerNos) : Server(Socket("4445")) {
    players = std::vector<std::pair<int, std::pair<std::string, std::string> > >();
    assert(playerNos > 0);
    noPlayers = playerNos;
  }

  void makeServerSocket() {
    Server.createSocket();
    Server.bindSocket();
    Server.listenOnSocket();
  }
  //This functino waits for incoming connections and assigns players automatically
  bool letPlayersConnect() {
    for (size_t i = 0; i < noPlayers; i++) {
      std::pair<int, std::string> temp = Server.acceptConnections();
      std::vector<char> portVect = Server.readBuffer(temp.first);
      std::string port(portVect.begin(), portVect.end());
      std::pair<std::string, std::string> ipPort = std::make_pair(temp.second, port);
      std::cout << port << " actually is the port\n";
      players.push_back(std::make_pair(temp.first, ipPort));
    }
    return true;
  }

  void assignNeighbors() {
    assert(players.size() == noPlayers);
    for (size_t i = 0; i < players.size() - 1; i++) {
      std::string ipaddrPort = players[i].second.first + "\n" + players[i].second.second;
      std::cout << "Start of iPport" << ipaddrPort << std::endl
                << "Hymm for the weekend\n";
      Server.sendToClient(players[i + 1].first, ipaddrPort);
    }
  }

  void sendToClient() {
    potato_t newPotato;
    newPotato.hops = 5;
    newPotato.vecSize = 0;
    for (size_t i = 0; i < 5; i++) {
      newPotato.traceVector[i] = (i + 1) * 10;
      newPotato.vecSize++;
    }
    int client = players[0].first;
    Server.sendPotato(client, &newPotato);
  }
};

int main() {
  RingMaster r1(1);
  r1.makeServerSocket();
  r1.letPlayersConnect();
  r1.sendToClient();
  return 0;
}
