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

// Definitions shard between the client and server.
//  Both have read and write permissions, since they're going to
//  be talking to each other.
#define SERVER_QUEUE_NAME   "/jw-coop-processes-server"
#define QUEUE_PERMISSIONS 0660  // like chmod values, user and owner can read and write to queue
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10   // leave some extra space after message
#define READY_WAIT_SEC 5 // Count to this amount before proceeding with segments of procedures
#define REPORT_SEND true // Report every message sent before sending

// Fields defined in common.cpp
extern mqd_t qd_server;
extern int pid;
extern std::string processName;
extern bool shuttingDown;
extern char inbuf [MSG_BUFFER_SIZE];
extern char outbuf [MSG_BUFFER_SIZE];
extern struct mq_attr attr;

// Function prototypes, defined in common.cpp
void ShutdownMQ(int signum); // This has a different contextual procedure on both instances

void introduce(void);
void assert(int code, std::string why);

void send(mqd_t mq_desc);
void listen(mqd_t mq_desc);

void countdown(void);

#endif