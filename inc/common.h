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

// Both instances expect Queue Descriptors for the server
//  However, the context for qd_client from both instances are completely different
mqd_t qd_server;

// Both instances have an input and output buffer.
char inbuf [MSG_BUFFER_SIZE];
char outbuf [MSG_BUFFER_SIZE];

// The attr struct will be shared between both instances
//  No real reason to make them different
struct mq_attr attr = {
    .mq_flags = 0,
    .mq_maxmsg = MAX_MESSAGES,
    .mq_msgsize = MAX_MSG_SIZE,
    .mq_curmsgs = 0
};

// Shorthand for the if-statement check when the mq result == -1.
//  Which means it failed. Prints off why and closes the application.
void mq_assert(int code, std::string why) {
    if (code == -1) {
        std::cerr << why << '\n';
        exit(1);
    }
}

// Countdown to READY_WAIT_SEC
void countdownWait() {
    int maxWaitSecs = READY_WAIT_SEC;
    for (int i = READY_WAIT_SEC; i > 0; i--) {
        std::cout << i << "... " << flush;
        usleep(1000000);
    }
    std::cout << '\n';
}

#endif