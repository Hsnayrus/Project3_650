CPPFLAGS = -Wall -Werror -pedantic -ggdb3

.PHONY: clean
 
all: client ringmaster client2

ringmaster: RingMaster.cpp
	g++  -o ringmaster $(CPPFLAGS) RingMaster.cpp

client: Client.cpp
	g++  -o client $(CPPFLAGS) Client.cpp

client2: Client2.cpp
	g++ -o  client2 $(CPPFLAGS) Client2.cpp	

clean:
	rm -rf ringmaster client client2