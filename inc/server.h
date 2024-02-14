#ifndef JW_COOPPROCESSES_SERVER
#define JW_COOPPROCESSES_SERVER
#include "common.h"

// Server constants
#define EXPECTED_CLIENTS 4

// Server fields
extern std::vector<std::string> clients;
extern std::map<std::string, mqd_t> openClients;
extern float initialClientTemps[EXPECTED_CLIENTS];
extern float serverCentralTemp;

// Server functions, defined in ServerProcedures.cpp
void SendToAllClients(void);
void WaitForClients(void);
void RunUntilClientsAreStable(void);


#endif