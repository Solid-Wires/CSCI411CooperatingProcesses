#ifndef JW_COOPPROCESSES_SERVER
#define JW_COOPPROCESSES_SERVER
#include "common.h"

// Server constants
#define EXPECTED_CLIENTS 4

// Server fields
extern std::vector<std::string> clients;
extern std::map<std::string, mqd_t> openClients;
extern int initialClientTemps[EXPECTED_CLIENTS];
extern int serverCentralTemp;

// Server functions, defined in ServerProcedures.cpp
int CentralTempCalculation(float extTempsSum);
void SendToAllClients(void);
void ShutdownClients(void);
void WaitForClients(void);
void RunUntilClientsAreStable(void);

#endif