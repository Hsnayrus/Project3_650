CPPFLAGS = -Wall -Werror -pedantic -ggdb3

all: client ringmaster

ringmaster: RingMaster.cpp
	g++ -o ringmaster $(CPPFLAGS) RingMaster.cpp

client: Client.cpp
	g++ -o client $(CPPFLAGS) Client.cpp