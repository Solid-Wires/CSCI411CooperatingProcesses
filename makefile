CLIENT=client
SERVER=server

OBJ_DIR=obj
CLIENT_OBJ_DIR=$(OBJ_DIR)/client
SERVER_OBJ_DIR=$(OBJ_DIR)/server
COMMON_OBJ_DIR=$(OBJ_DIR)/common

# There we go, now it's compiling to the right version
VERSION=-std=c++11
REAL_TIME_LIBRARY=-lrt # Needed because of mqueue.h

all: directories programs

# Make all of these directories if they don't exist (silent)
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(CLIENT_OBJ_DIR)
	@mkdir -p $(SERVER_OBJ_DIR)
	@mkdir -p $(COMMON_OBJ_DIR)

client_comp: src/client.cpp
	gcc $(VERSION) -c src/client.cpp -o $(CLIENT_OBJ_DIR)/client.o

server_comp: src/server.cpp
	gcc $(VERSION) -c src/server.cpp -o $(SERVER_OBJ_DIR)/server.o

common_comps: src/common.cpp
	gcc $(VERSION) -c src/common.cpp -o $(COMMON_OBJ_DIR)/common.o

programs: client_comp server_comp common_comps
	g++ obj/client.o $(COMMON_OBJ_DIR)/*.o -o $(CLIENT) $(REAL_TIME_LIBRARY)
	g++ obj/server.o  $(COMMON_OBJ_DIR)/*.o -o $(SERVER) $(REAL_TIME_LIBRARY)

.PHONY: clean
clean:
	rm -f $(CLIENT)
	rm -f $(SERVER)
	rm -f obj/*.o

