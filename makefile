CLIENT=CoopProcessClient
SERVER=CoopProcessServer

# There we go, now it's compiling to the right version
VERSION=-std=c++11
REAL_TIME_LIBRARY=-lrt # Needed because of mqueue.h

all: programs

client_comp: src/client.cpp
	gcc $(VERSION) -c src/client.cpp -o obj/client.o

server_comp: src/server.cpp
	gcc $(VERSION) -c src/server.cpp -o obj/server.o

# There might be a shared compilation

programs: client_comp server_comp
	g++ obj/client.o -o $(CLIENT) $(REAL_TIME_LIBRARY)
	g++ obj/server.o -o $(SERVER) $(REAL_TIME_LIBRARY)

.PHONY: clean
clean:
	rm -f $(CLIENT)
	rm -f $(SERVER)
	rm -f obj/*.o

