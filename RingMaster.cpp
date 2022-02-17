/*
This code was made after reading the following article:

https://beej.us/guide/bgnet/html//index.html


I have also taken references from the 07-tcp_example.zip file available
in the Resources tab in Sakai
*/

#include "Socket.hpp"

int main() {
  Socket s1("4445");
  s1.createSocket();
  s1.bindSocket();
  s1.listenOnSocket();
  while (true) {
    s1.acceptConnections(100);
  }
}