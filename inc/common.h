#ifndef JW_COOPPROCESSES_COMMON
#define JW_COOPPROCESSES_COMMON

// These includes will also be included between the client
//  and server - so no need to restate them.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <cstring>
#include <iostream>

// Definitions shard between the client and server.
#define SERVER_QUEUE_NAME   "/jw-coop-processes-server"
#define QUEUE_PERMISSIONS 0660  // like chmod values, user and owner can read and write to queue
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10   // leave some extra space after message

struct mq_attr attr = {
    .mq_flags = 0,
    .mq_maxmsg = MAX_MESSAGES,
    .mq_msgsize = MAX_MSG_SIZE,
    .mq_curmsgs = 0
};

#endif