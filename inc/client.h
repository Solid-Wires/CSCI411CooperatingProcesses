#ifndef JW_COOPPROCESSES_CLIENT
#define JW_COOPPROCESSES_CLIENT
#include "common.h"

// Client constants
#define CLIENT_QUEUE_NAME   "/jw-coop-processes-client-%d"

// Client fields
mqd_t qd_client;
char client_queue_name[MAX_CLIENT_NAME_SIZE];
float clientExtTemp;

// Client functions, defined by ClientProcedures.cpp
void GreetAndAwaitInitiationResponseFromServer(void);

#endif