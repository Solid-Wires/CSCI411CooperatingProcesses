CLIENT=CoopProcessClient
SERVER=CoopProcessServer

client_comp: src/client.cpp
	gcc -c src/client.cpp -o obj/client.o

server_comp: src/server.cpp
	gcc -c src/server.cpp -o obj/server.o

# There might be a shared compilation

all: client_comp server_comp
	g++ obj/client.o -o $(CLIENT)
	g++ obj/server.o -o $(SERVER)

.PHONY: clean
clean:
	rm -f $(CLIENT)
	rm -f $(SERVER)
	rm -f obj/*.o
