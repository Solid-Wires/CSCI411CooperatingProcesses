CLIENT=CoopProcessClient
SERVER=CoopProcessServer

VERSION=c++11

all: programs

client_comp: src/client.cpp
	gcc -std=$(VERSION) -c src/client.cpp -o obj/client.o

server_comp: src/server.cpp
	gcc -std=$(VERSION) -c src/server.cpp -o obj/server.o

# There might be a shared compilation

programs: client_comp server_comp
	g++ obj/client.o -o $(CLIENT)
	g++ obj/server.o -o $(SERVER)

.PHONY: clean
clean:
	rm -f $(CLIENT)
	rm -f $(SERVER)
	rm -f obj/*.o

