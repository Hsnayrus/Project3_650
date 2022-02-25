

#include "Socket.hpp"
#include "functions.hpp"
#include "synchronize.hpp"

void simpleAccept(Socket & leftPlayer, std::pair<int, std::string> & leftsClient) {
  leftsClient = leftPlayer.acceptConnections();
}

void tryConnectTo(Socket & rightPlayer) {
  rightPlayer.connectTo(rightPlayer.getSocket_FD());
}

int main(int argc, char ** argv) {
  if (argc != 3) {
    std::cerr << "Wrong usage. Correct usage is ./player machineName pprtNumber\n";
    return 0;
  }

  /*
Referred to http://www.cplusplus.com/forum/articles/9742/
for this piece of code before Socket client() line
*/

  hostent * record = gethostbyname(argv[1]);
  if (record == NULL) {
    std::cout << argv[1] << " is unavailable\n";
    return 0;
  }

  const char * ringMasterPort = argv[2];
  in_addr * address = (in_addr *)record->h_addr;
  std::string ip_address = inet_ntoa(*address);

  Socket client(ip_address.c_str(), ringMasterPort);
  std::string messageToSend = "Ready for connection.";
  std::vector<char> newVector(messageToSend.begin(),
                              messageToSend.begin() + messageToSend.size());
  client.createSocket();
  client.sendToServer(newVector);

  int ringMaster_fd = client.getSocket_FD();

  newVector.clear();
  cInfo_t info;
  client.receiveClientInfo(client.getSocket_FD(), info);

  int myID = info.portNum - 31000;
  int leftsID = INT8_MIN;
  int rightsID = INT8_MIN;
  if (myID == 0) {
    leftsID = info.noPlayers - 1;
    rightsID = myID + 1;
  }
  else if (myID == info.noPlayers - 1) {
    leftsID = myID - 1;
    rightsID = 0;
  }
  else {
    leftsID = myID - 1;
    rightsID = myID + 1;
  }

  //Server that waits for a connection
  Socket leftPlayer(num2Str(info.portNum).c_str());
  leftPlayer.createSocket();
  leftPlayer.bindSocket();
  leftPlayer.listenOnSocket();

  sync_t syncInfo;
  syncInfo.doneListening = 1;
  syncInfo.startAccepting = 0;
  syncInfo.doneAccepting = 0;

  client.sendSyncInfo(ringMaster_fd, &syncInfo);
  client.receiveSyncInfo(ringMaster_fd, syncInfo);
  //Client that establishes a connection with the other player
  Socket rightPlayer(info.ipAddress, num2Str(info.nPort).c_str());
  rightPlayer.createSocket();
  if (syncInfo.startAccepting == 1) {
    rightPlayer.connectTo(rightPlayer.getSocket_FD());
    std::cout << "Connected as player " << myID << " out of " << info.noPlayers
              << " total players\n";

    std::pair<int, std::string> leftsClient = leftPlayer.acceptConnections();
    syncInfo.doneAccepting = 1;
    client.sendSyncInfo(client.getSocket_FD(), &syncInfo);

    // potato_t potato;
    // potato.hops = 0;
    // potato.vecSize = 0;
    // if (myID == 0) {
    //   recv(client.getSocket_FD(), &potato, sizeof(potato), 0);
    //   send(leftsClient.first, &potato, sizeof(potato), 0);
    // }
    // if (myID == 2) {
    //   recv(rightPlayer.getSocket_FD(), &potato, sizeof(potato), 0);
    // }
    // std::cout << "Client: " << myID << " received : " << std::endl;
    // for (size_t i = 0; i < potato.vecSize; i++) {
    //   std::cout << potato.traceVector[i] << ",";
    // }
    // std::cout << std::endl << potato.hops << std::endl;

    int leftClient_fd = leftsClient.first;
    int rightClient_fd = rightPlayer.getSocket_FD();
    std::vector<int> fds;
    fds.push_back(leftClient_fd);
    fds.push_back(rightClient_fd);
    fds.push_back(ringMaster_fd);

    srand((unsigned int)time(NULL) + myID);
    while (true) {
      potato_t potato;
      potato.hops = 0;
      potato.vecSize = 0;
      memset(potato.traceVector, 0, sizeof(potato.traceVector));

      fd_set fdset;
      int fdmax = findMax(fds);
      FD_ZERO(&fdset);
      for (size_t i = 0; i < fds.size(); i++) {
        FD_SET(fds[i], &fdset);
      }

      // std::cout << "Coming in main while loop\n";

      int sendTo = rand() % 2;

      // std::cout << "Client: " << myID << " received : " << std::endl;
      // for (size_t i = 0; i < potato.vecSize; i++) {
      //   std::cout << potato.traceVector[i] << ",";
      // }
      // std::cout << std::endl << potato.hops << std::endl;

      select(fdmax + 1, &fdset, NULL, NULL, NULL);

      size_t len = INT8_MIN;
      for (size_t i = 0; i < fds.size(); i++) {
        if (FD_ISSET(fds[i], &fdset)) {
          len = recv(fds[i], &potato, sizeof(potato), MSG_WAITALL);
          break;
        }
      }
      if (len == sizeof(size_t)) {
        break;
      }
      potato.traceVector[potato.vecSize] = myID;
      potato.vecSize++;
      if (potato.hops == potato.vecSize) {
        size_t done = 0;
        send(ringMaster_fd, &potato, sizeof(potato), 0);
        std::cout << "I'm it\n";
        len = recv(ringMaster_fd, &done, sizeof(done), MSG_WAITALL);
        if (len == sizeof(done)) {
          std::cout << "Received done: " << done << std::endl;
          break;
        }
      }
      // bool flag = true;
      if (sendTo == 0) {
        send(leftClient_fd, &potato, sizeof(potato), 0);
        std::cout << "Sending potato to " << leftsID << "\n";
      }
      else if (sendTo == 1) {
        send(rightClient_fd, &potato, sizeof(potato), 0);
        std::cout << "Sending potato to " << rightsID << "\n";
      }
    }
  }
  // while (true) {
  // select(fdmax + 1, &fdset, NULL, NULL, NULL);
  // sendTo = rand() % 2;
  // potato_t potato;
  // potato.hops = 0;
  // potato.vecSize = 0;
  // if (FD_ISSET(leftClient_fd, &fdset)) {
  //   recv(leftClient_fd, &potato, sizeof(potato), MSG_WAITALL);
  // }
  // if (FD_ISSET(rightClient_fd, &fdset)) {
  //   recv(rightClient_fd, &potato, sizeof(potato), MSG_WAITALL);
  // }
  // if (FD_ISSET(ringMaster_fd, &fdset)) {
  //   std::cout << "Received from ringmaster\n";
  //   recv(ringMaster_fd, &potato, sizeof(potato), MSG_WAITALL);
  // }
  // potato.traceVector[potato.vecSize] = myID;
  // potato.vecSize++;
  // if (potato.vecSize == potato.hops) {
  //   std::cout << "I'm it\n";
  //   send(ringMaster_fd, &potato, sizeof(potato), 0);
  // }
  // if (sendTo == 0) {
  //   send(leftClient_fd, &potato, sizeof(potato), 0);
  //   std::cout << "Sent to left client" << sendTo << "\n";
  // }
  // if (sendTo == 1) {
  //   send(rightClient_fd, &potato, sizeof(potato), 0);
  //   std::cout << "Sent to right client\n" << sendTo << "\n";
  // }
  // std::cout << "Client: " << myID << " received : " << std::endl;
  // for (size_t i = 0; i < potato.vecSize; i++) {
  //   std::cout << potato.traceVector[i] << ",";
  // }
  // std::cout << std::endl << potato.hops << std::endl;

  // if (potato.vecSize != potato.hops) {
  //   while(true){
  //     select(fdmax + 1, &fdset, NULL, NULL, NULL);
  //     if(FD_ISSET(leftClient_fd, &fdset)){
  //       recv()
  //     }
  //   }
  // }
  /*

*/

  //   while (flag) {
  //     select(fdmax + 1, &fdset, NULL, NULL, NULL);
  //     sendTo = rand() % 2;
  //     potato_t potato;
  //     potato.hops = 0;
  //     potato.vecSize = 0;
  //     if (FD_ISSET(leftClient_fd, &fdset)) {
  //       recv(leftClient_fd, &potato, sizeof(potato), MSG_WAITALL);
  //     }
  //     if (FD_ISSET(rightClient_fd, &fdset)) {
  //       recv(rightClient_fd, &potato, sizeof(potato), MSG_WAITALL);
  //     }
  //     if (FD_ISSET(ringMaster_fd, &fdset)) {
  //       std::cout << "Received from ringmaster\n";
  //       recv(ringMaster_fd, &potato, sizeof(potato), MSG_WAITALL);
  //     }
  //     potato.traceVector[potato.vecSize] = myID;
  //     potato.vecSize++;
  //     if (potato.vecSize == potato.hops) {
  //       std::cout << "I'm it\n";
  //       send(ringMaster_fd, &potato, sizeof(potato), 0);
  // flag = false;
  //     }
  //     if (sendTo == 0) {
  //       send(leftClient_fd, &potato, sizeof(potato), 0);
  //       std::cout << "Sent to left client" << sendTo << "\n";
  //     }
  //     if (sendTo == 1) {
  //       send(rightClient_fd, &potato, sizeof(potato), 0);
  //       std::cout << "Sent to right client\n" << sendTo << "\n";
  //     }
  //     // std::cout << "Client: " << myID << " received : " << std::endl;
  //     // for (size_t i = 0; i < potato.vecSize; i++) {
  //     //   std::cout << potato.traceVector[i] << ",";
  //     // }
  //     // std::cout << std::endl << potato.hops << std::endl;
  //   }
  // }
  return 0;
}

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

// std::pair<int, int> parseIDNoPlayers(std::string message) {
//   size_t start = message.find("//////");
//   int id = str2num(message.substr(0, start));
//   int noPlayers = str2num(message.substr(start + 6));
//   return std::make_pair(id, noPlayers);
// }

// std::pair<std::string, std::string> parseIPPort(std::string message) {
//   size_t start = message.find("//////");
//   std::string ip = message.substr(0, start);
//   std::cout << "==========================================\n";
//   std::cout << message << message.size() << std::endl;

//   std::cout << "==========================================\n";
//   std::string port = message.substr(start + 6);
//   return std::make_pair(ip, port);
// }
