/*
This code was made after reading the following article:

https://beej.us/guide/bgnet/html//index.html


I have also taken references from the 07-tcp_example.zip file available
in the Resources tab in Sakai.
*/

#include <cassert>

#include "Socket.hpp"
#include "client.hpp"
#include "functions.hpp"

int main(int argc, char ** argv) {
  if (argc != 4) {
    std::cerr
        << "Wrong usage. Correct usage is: ./ringmaster portnumber numPlayers numHops\n";
    return 0;
  }
  int noPlayersInt = atoi(argv[2]);
  int noHopsInt = atoi(argv[3]);
  assert(noPlayersInt > 1);
  assert(noHopsInt >= 0 && noHopsInt < 512);

  size_t noPlayers = (size_t)noPlayersInt;
  size_t noHops = (size_t)noHopsInt;
  const char * portNumber = argv[1];
  std::cout << "Potato Ringmaster\nPlayers = " << noPlayers << "\nHops = " << noHops
            << "\n";
  Socket ringMaster(portNumber);
  ringMaster.createSocket();
  ringMaster.bindSocket();
  ringMaster.listenOnSocket();
  std::vector<cInfo_t> clientsInformation;
  std::vector<char> buffer(65536);
  std::vector<std::string> ipAddresses;
  std::vector<int> nPorts;
  // buffer.clear();
  //Accept players;
  for (size_t i = 0; i < noPlayers; i++) {
    std::pair<int, std::string> temp = ringMaster.acceptConnections();
    // std::cout << "Accept players loop " << temp.first << "\n";
    int len = ringMaster.readBuffer(temp.first, buffer);

    std::string message(buffer.begin(), buffer.begin() + len);
    cInfo_t clientInfo;
    clientInfo.fd = temp.first;
    clientInfo.portNum = 31000 + i;
    clientInfo.noPlayers = noPlayers;
    ipAddresses.push_back(temp.second);
    nPorts.push_back(clientInfo.portNum);
    if (message.find("Ready for connection.") != std::string::npos) {
      clientsInformation.push_back(clientInfo);
    }
  }
  // std::cout << clientsInformation.size() << "&&&&\n";
  //Basically i am appending the list of ip addresss to itself;
  //Then i will just assign target addresses to players starting from index 1.
  //This is a cleaner approach
  size_t size = ipAddresses.size();
  for (size_t i = 0; i < size; i++) {
    ipAddresses.push_back(ipAddresses[i]);
    nPorts.push_back(nPorts[i]);
  }
  //Loop to set player's target ip addresses
  for (size_t i = 0; i < clientsInformation.size(); i++) {
    (clientsInformation[i]).ipSize = ipAddresses[i + 1].size();
    (clientsInformation[i]).nPort = nPorts[i + 1];
    for (size_t j = 0; j < ipAddresses[i + 1].size(); j++) {
      (clientsInformation[i]).ipAddress[j] = (ipAddresses[i + 1])[j];
    }
  }

  //Loop to send client's their portNumber, their fd's and their neighbors ip Addresses
  for (size_t i = 0; i < clientsInformation.size(); i++) {
    // std::cout << "Coming in this loop\n";
    cInfo_t temp;
    temp.fd = clientsInformation[i].fd;
    temp.portNum = clientsInformation[i].portNum;
    temp.ipSize = clientsInformation[i].ipSize;
    for (int j = 0; j < temp.ipSize; j++) {
      temp.ipAddress[j] = (clientsInformation[i]).ipAddress[j];
    }
    temp.nPort = clientsInformation[i].nPort;
    temp.noPlayers = clientsInformation[i].noPlayers;
    ringMaster.sendClientInfo(temp.fd, &temp);
    temp.fd = clientsInformation[i].fd;
    // std::cout << "%%%%%%%%%%%%%%%%\n";
    // std::cout << temp.fd << " " << temp.ipSize << " " << temp.portNum << std::endl;
    // for (int i = 0; i < temp.ipSize; i++) {
    //   std::cout << temp.ipAddress[i];
    // }
  }

  //Accept client status, listening information from players
  std::vector<sync_t> clientSync;
  for (size_t i = 0; i < noPlayers; i++) {
    sync_t temp;
    temp.doneAccepting = 0;
    temp.doneListening = 0;
    temp.startAccepting = 1;
    ringMaster.receiveSyncInfo(clientsInformation[i].fd, temp);
    temp.startAccepting = 1;
    clientSync.push_back(temp);
  }

  //Check if all are done listening
  size_t allDoneListening = 0;
  while (allDoneListening != noPlayers) {
    for (size_t i = 0; i < noPlayers; i++) {
      if (clientSync[i].doneListening == 1) {
        clientSync[i].doneListening = 0;
        allDoneListening++;
      }
    }
  }
  //Tell clients to start accepting connections
  for (size_t i = 0; i < noPlayers; i++) {
    sync_t temp;
    temp.doneAccepting = clientSync[i].doneAccepting;
    temp.doneListening = 1;
    temp.startAccepting = clientSync[i].startAccepting;
    ringMaster.sendSyncInfo(clientsInformation[i].fd, &temp);
  }

  //Receive ack that everyone has started accepting connections
  for (size_t i = 0; i < noPlayers; i++) {
    sync_t temp;
    ringMaster.receiveSyncInfo(clientsInformation[i].fd, temp);
    clientSync[i].doneAccepting = temp.doneAccepting;
    clientSync[i].doneListening = temp.doneListening;
    clientSync[i].startAccepting = temp.startAccepting;
  }
  size_t allAccepted = 0;
  while (allAccepted != noPlayers) {
    for (size_t i = 0; i < noPlayers; i++) {
      if (clientSync[i].doneAccepting == 1) {
        clientSync[i].doneAccepting = 0;
        allAccepted++;
      }
    }
  }
  if (allDoneListening == noPlayers && allAccepted == noPlayers) {
    // std::cout << "All clients accepted it seems\n";
    for (size_t i = 0; i < noPlayers; i++) {
      std::cout << "Player " << i << " is ready to play\n";
    }
  }
  // int value = 9;
  potato_t potato;
  potato.hops = noHops;
  potato.vecSize = 0;
  memset(potato.traceVector, 0, sizeof(potato));

  int playerToStartWith = rand() % noPlayers;
  // std::cout << "Starting game with player " << playerToStartWith << std::endl;

  // std::cout << "###################\n";
  // std::cout << potato.hops << " " << potato.vecSize << std::endl;
  // for (size_t i = 0; i < potato.vecSize; i++) {
  //   std::cout << potato.traceVector[i] << ", ";
  // }
  // std::cout << "\n###################\n";
  std::cout << "Ready to start the game, sending potato to player " << playerToStartWith
            << std::endl;
  if (noHops != 0) {
    int status =
        send(clientsInformation[playerToStartWith].fd, &potato, sizeof(potato), 0);
    if (status == -1) {
      std::cerr << "Couldn't send potato to player: " << playerToStartWith << std::endl;
    }
    std::vector<int> fds;

    fd_set fdset;
    FD_ZERO(&fdset);
    for (size_t i = 0; i < clientsInformation.size(); i++) {
      FD_SET(clientsInformation[i].fd, &fdset);
      fds.push_back(clientsInformation[i].fd);
    }
    int fdmax = findMax(fds);

    select(fdmax + 1, &fdset, NULL, NULL, NULL);
    for (size_t i = 0; i < fds.size(); i++) {
      if (FD_ISSET(fds[i], &fdset)) {
        recv(fds[i], &potato, sizeof(potato), MSG_WAITALL);
        break;
      }
    }
    size_t done = 99;
    for (size_t i = 0; i < noPlayers; i++) {
      send(fds[i], &done, sizeof(done), 0);
    }
    // std::cout << "###################\n";
    // std::cout << potato.hops << " " << potato.vecSize << std::endl;
    for (size_t i = 0; i < potato.vecSize; i++) {
      std::cout << potato.traceVector[i];
      if (i != potato.vecSize - 1) {
        std::cout << ",";
      }
    }
    // std::cout << "\n###################\n";
  }
  else {
    size_t done = 99;
    for (size_t i = 0; i < clientsInformation.size(); i++) {
      send(clientsInformation[i].fd, &done, sizeof(done), 0);
    }
  }
  return 0;
}

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
