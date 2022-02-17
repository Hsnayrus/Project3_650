#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

typedef struct addrinfo AddressInfo;

class Socket {
  int status;
  int socket_fd;
  AddressInfo host_info;
  AddressInfo * host_info_list;
  const char * hostName = NULL;
  const char * port = NULL;

 public:
  /*
This constructor takes in two std:strings and assigns hostname and port accordingly.
@params(std::string hostname, std::string portNumber)
*/
  Socket(std::string & newHostName, std::string & newPort) {
    hostName = newHostName.c_str();
    port = newPort.c_str();
  }

  /*
This constructor takes in two const char pointers and assigns hostname and port accordingly.
@params(const char * hostname, const char * portNumber)
*/
  Socket(const char * newHost, const char * newPort) {
    hostName = newHost;
    port = newPort;
  }
  /*
This constructor is to be utilized by the ringmaster because it doesnt actually have to connect 
to a host. The host is self hence the hostName field should stay NULL
@params const char * port. Initializes a socket object with the specified port number
*/
  Socket(const char * newPort) {
    port = newPort;
    hostName = INADDR_ANY;
  }

  /*
This constructor is to be utilized by the ringmaster because it doesnt actually have to connect 
to a host. The host is self hence the hostName field should stay 0.0.0.0 as suggested by project guidelines
@params std::string port. Initializes a socket object with the specified port number
*/
  Socket(std::string & newPort) {
    port = newPort.c_str();
    hostName = INADDR_ANY;
  }

  /*
This function creates a socket with the hostname and socket initialized before while object creation
@returns -1 if there was error in creating a socket. 
@returns 0 if socket was created successfully
@displays appropriate error message on receiving error
*/
  int createSocket() {
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    status = getaddrinfo(hostName, port, &host_info, &host_info_list);
    if (status != 0) {
      std::cerr << "Error in getting address information for host: " << hostName
                << " at port: " << port << std::endl;
      return -1;
    }

    socket_fd = socket(host_info_list->ai_family,
                       host_info_list->ai_socktype,
                       host_info_list->ai_protocol);

    if (socket_fd == -1) {
      std::cerr << "Error in creating a socket with host: " << hostName
                << " at port: " << port << std::endl;
      return -21;
      ;
    }

    return 0;
  }
  /*
This function is responsible for binding the socket to a file descriptor on our local machine.
It will enable us to listen for incoming requests on the socket
@returns 0 if binding successful, -1 otherwise
*/
  int bindSocket() {
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status != 0) {
      std::cerr << "Error in binding socket\n";
      return -1;
    }
    return status;
  }

  int listenOnSocket() {
    //1020 limit as suggested by project guidelines.
    status = listen(socket_fd, 1020);
    if (status != 0) {
      std::cerr << "Error in listening on socket with hostname " << hostName
                << " and port: " << port << std::endl;
    }
    return status;
  }

  /*
Method that allows a socket to receive data from clients
*/
  int acceptConnections(int noConnections) {
    int client_connection_fd;
    // while (noConnections != 0) {
    std::cout << "Waiting for connection on port: " << port << std::endl;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    client_connection_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
      std::cerr << "Error: Cannot accept connection on socket\n";
      return -1;
    }
    // }
    readBuffer(client_connection_fd);
    return client_connection_fd;
  }
  //Reads from the buffer
  std::vector<char> readBuffer(int client_connection_fd) {
    std::vector<char> buffer(1024 * 1024);
    recv(client_connection_fd, buffer.data(), buffer.size(), 0);
    std::cout << "Server received: ";
    for (size_t i = 0; i < buffer.size(); i++) {
      std::cout << buffer[i];
    }
    std::cout << buffer.size() << std::endl;
    std::cout << std::endl;
    return buffer;
  }

  /*
Method that allows user to send data to the server
*/
  void sendToServer(std::vector<char> request) {
    std::cout << "Connecting to " << hostName << " on port " << port << "..."
              << std::endl;

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      std::cerr << "Error: cannot connect to socket" << std::endl;
      std::cerr << "  (" << hostName << "," << port << ")" << std::endl;
    }  //if

    const char * message = request.data();
    send(socket_fd, message, strlen(message), 0);
  }
};
