#ifndef JW_COOPPROCESSES_COMMON
#define JW_COOPPROCESSES_COMMON

// These includes will also be included between the client
//  and server - so no need to restate them.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <mqueue.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

// Common constants
#define SERVER_QUEUE_NAME   "/jw-coop-processes-server"
#define QUEUE_PERMISSIONS 0660  // like chmod values, user and owner can read and write to queue
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256 // The messages probably don't need to be that big, but I'm keeping it this way so it still works
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10   // leave some extra space after message
#define MAX_CLIENT_NAME_SIZE MAX_MSG_SIZE
#define READY_WAIT_SEC 5 // Count to this amount before proceeding with segments of procedures
#define REPORT_SEND true // Report every message sent before sending
#define CLIENT_END_MESSAGE   "END"
#define TEMP_DEC_PLACES 8   // Display temperatures to 8 decimal places

// Common fields
extern mqd_t qd_server;
extern int pid;
extern std::string processName;
extern bool shuttingDown;
extern char inbuf [MSG_BUFFER_SIZE];
extern char outbuf [MSG_BUFFER_SIZE];
extern struct mq_attr attr;

// Common functions, mostly defined in Common.cpp
void ShutdownMQ(int signum); // Defined on Client.cpp and Server.cpp

void introduce(void);
void assert(int code, std::string why);

void send(mqd_t mq_desc);
void listen(mqd_t mq_desc);

void countdown(void);

#endif