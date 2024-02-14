#ifndef JW_COOPPROCESSES_CLIENT
#define JW_COOPPROCESSES_CLIENT
#include "common.h"

// Client constants
#define CLIENT_QUEUE_NAME   "/jw-coop-processes-client-%d"

// Client fields
extern mqd_t qd_client;
extern char client_queue_name[MAX_CLIENT_NAME_SIZE];
extern float clientExtTemp;

// Client functions, defined by ClientProcedures.cpp
float ExternalTempCalculation(float centralTemp);
void GreetAndAwaitInitiationResponseFromServer(void);
void ListenForCentralTempAndUpdateExternalTemp(void);

#endif