/*
This code was made after reading the following article:

https://beej.us/guide/bgnet/html//index.html


I have also taken references from the 07-tcp_example.zip file available
in the Resources tab in Sakai
*/

#include <cassert>

#include "Socket.hpp"
#include "client.hpp"
#include "functions.hpp"
// class RingMaster {
//   Socket Server;
//   std::vector<std::pair<int, std::pair<std::string, std::string> > > players;
//   size_t noPlayers;

//  public:
//   RingMaster() : Server(Socket("4445")) {
//     players = std::vector<std::pair<int, std::pair<std::string, std::string> > >();
//     noPlayers = 0;
//     makeServerSocket();
//   }

//   RingMaster(size_t playerNos) : Server(Socket("4445")) {
//     players = std::vector<std::pair<int, std::pair<std::string, std::string> > >();
//     assert(playerNos > 0);
//     noPlayers = playerNos;
//   }

//   void makeServerSocket() {
//     Server.createSocket();
//     Server.bindSocket();
//     Server.listenOnSocket();
//   }
//   //This functino waits for incoming connections and assigns players automatically/
//   //std::vector<std::pair<int, std::pair<std::string, std::string> > > playersis in the format:
//   //clientfd, <ipAddress, port>
//   bool letPlayersConnect() {
//     for (size_t i = 0; i < noPlayers; i++) {
//       std::pair<int, std::string> temp = Server.acceptConnections();
//       std::vector<char> portVect = Server.readBuffer(temp.first);
//       int port = 2000 + i;
//       std::string messageFromClient(portVect.begin(), portVect.end());
//       if (messageFromClient.find("Ready for connection.") != std::string::npos) {
//         std::string portStr = num2Str(port);
//         players.push_back(
//             std::make_pair(temp.first, std::make_pair(temp.second, portStr)));
//       }
//     }
//     return true;
//   }

//   void assignNeighbors() {
//     assert(players.size() == noPlayers);
//     std::string message = "";
//     for (size_t i = 0; i < players.size() - 1; i++) {
//       message.clear();
//       message =
//           players[i + 1].second.first + "//////" + players[i + 1].second.second + "\n";
//       std::vector<char> loopBuffer(message.begin(), message.end());
//       send(players[i].first, loopBuffer.data(), message.size(), 0);
//     }
//     message.clear();
//     message = players[0].second.first + "//////" + players[0].second.second + "\n";
//     std::string buffer(message.begin(), message.end());
//     send(players[players.size() - 1].first, buffer.data(), message.size(), 0);
//   }

//   //Send ports. along with no of players to client
//   //ports = id + 2000
//   void sendIDsToPlayers() {
//     for (size_t i = 0; i < players.size(); i++) {
//       std::string message =
//           (players[i].second.second) + "//////" + num2Str(noPlayers) + "\n";
//       std::vector<char> buffer(message.begin(), message.end());
//       //Sending just their ids
//       int status = send(players[i].first, buffer.data(), message.size(), 0);
//       if (status == -1) {
//         std::cerr << "RingMaster/sendIDs, Couldn;t send ids\n";
//         return;
//       }
//       buffer.clear();
//     }
//   }

//   void sendPotatoToClient() {
//     potato_t newPotato;
//     newPotato.hops = 5;
//     newPotato.vecSize = 0;
//     for (size_t i = 0; i < 5; i++) {
//       newPotato.traceVector[i] = (i + 1) * 10;
//       newPotato.vecSize++;
//     }
//     int client = players[0].first;
//     Server.sendPotato(client, &newPotato);
//   }
// };

int main() {
  Socket ringMaster("4448");
  ringMaster.createSocket();
  ringMaster.bindSocket();
  ringMaster.listenOnSocket();
  std::vector<cInfo_t> clientsInformation;
  int noPlayers = 3;
  std::vector<char> buffer(65536);
  // buffer.clear();
  //Accept players;
  for (int i = 0; i < noPlayers; i++) {
    std::pair<int, std::string> temp = ringMaster.acceptConnections();
    std::cout << "Accept players loop " << temp.first << "\n";
    int len = ringMaster.readBuffer(temp.first, buffer);

    std::string message(buffer.begin(), buffer.begin() + len);
    cInfo_t clientInfo;
    clientInfo.fd = temp.first;
    clientInfo.portNum = 2000 + i;
    clientInfo.ipSize = 0;
    //Will be changed later
    for (size_t j = 0; j < (temp.second).size(); j++) {
      clientInfo.ipAddress[j] = (temp.second)[j];
      clientInfo.ipSize++;
    }
    if (message.find("Ready for connection.") != std::string::npos) {
      clientsInformation.push_back(clientInfo);
    }
  }
  std::cout << clientsInformation.size() << "&&&&\n";
  for (size_t i = 0; i < clientsInformation.size(); i++) {
    std::cout << "Coming in this loop\n";
    cInfo_t temp;
    temp.fd = clientsInformation[i].fd;
    temp.portNum = clientsInformation[i].portNum;
    temp.ipSize = clientsInformation[i].ipSize;
    for (int j = 0; j < temp.ipSize; j++) {
      temp.ipAddress[j] = (clientsInformation[i]).ipAddress[j];
    }
    ringMaster.sendClientInfo(temp.fd, &temp);
    temp.fd = clientsInformation[i].fd;
    std::cout << "%%%%%%%%%%%%%%%%\n";
    std::cout << temp.fd << " " << temp.ipSize << " " << temp.portNum << std::endl;
    for (int i = 0; i < temp.ipSize; i++) {
      std::cout << temp.ipAddress[i];
    }
  }
  //Send neihbors to players

  // RingMaster r1(2);
  // r1.makeServerSocket();
  // r1.letPlayersConnect();
  // r1.sendIDsToPlayers();
  // r1.assignNeighbors();
  // r1.sendPotatoToClient();
  return 0;
}