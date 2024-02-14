# Executable names
CLIENT=client
SERVER=server

# Directories
OBJ_DIR=obj
BIN_DIR=bin
CLIENT_OBJ_DIR=$(OBJ_DIR)/client
SERVER_OBJ_DIR=$(OBJ_DIR)/server
COMMON_OBJ_DIR=$(OBJ_DIR)/common

# There we go, now it's compiling to the right version
CC=cc
VERSION=-std=c++11
REAL_TIME_LIBRARY=-lrt # Needed because of mqueue.h

# All call
all: directories programs

# Make all of these directories if they don't exist
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(CLIENT_OBJ_DIR)
	@mkdir -p $(SERVER_OBJ_DIR)
	@mkdir -p $(COMMON_OBJ_DIR)

# Client and server object compilations
#	The subst function just replaces the % in the command with the name provided
client_comp: src/Client.cpp src/ClientProcedures.cpp
	$(subst %,Client, $(CC) $(VERSION) -c src/%.cpp -o $(CLIENT_OBJ_DIR)/%.o)
	$(subst %,ClientProcedures, gcc $(VERSION) -c src/%.cpp -o $(CLIENT_OBJ_DIR)/%.o)
server_comp: src/Server.cpp src/ServerProcedures.cpp
	$(subst %,Server, $(CC) $(VERSION) -c src/%.cpp -o $(SERVER_OBJ_DIR)/%.o)
	$(subst %,ServerProcedures, gcc $(VERSION) -c src/%.cpp -o $(SERVER_OBJ_DIR)/%.o)

# Common object compilation
common_comp: src/Common.cpp
	$(subst %,Common, $(CC) $(VERSION) -c src/%.cpp -o $(COMMON_OBJ_DIR)/%.o)

# Program executable compilation
programs: client_comp server_comp common_comp
	g++ $(CLIENT_OBJ_DIR)/*.o $(COMMON_OBJ_DIR)/*.o -o $(BIN_DIR)/$(CLIENT) $(REAL_TIME_LIBRARY)
	g++ $(SERVER_OBJ_DIR)/*.o  $(COMMON_OBJ_DIR)/*.o -o $(BIN_DIR)/$(SERVER) $(REAL_TIME_LIBRARY)

# Clean does a recursive removal of the generated bin and obj directories.
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

