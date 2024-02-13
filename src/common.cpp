#include "../inc/common.h"
using namespace std;

// Both instances expect Queue Descriptors for the server
//  However, the context for qd_client from both instances are completely different
mqd_t qd_server;

// The pid and custom name of the instance, stored.
//  processName is mainly used for debugging
int pid;
std::string processName;

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

// Print out what the process is sending before sending it.
//  Part of the assignment's reqs.
void reportSend() {
    if (REPORT_SEND) {
        cout << pid << ": " << outbuf;
    }
}

// Shorthand for the if-statement check when the mq result == -1.
//  Which means it failed. Prints off why and closes the application.
void mq_assert(int code, string why) {
    if (code == -1) {
        cerr << why << '\n';
        exit(1);
    }
}

// Countdown to READY_WAIT_SEC
void countdownWait() {
    int maxWaitSecs = READY_WAIT_SEC;
    for (int i = READY_WAIT_SEC; i > 0; i--) {
        cout << i << "... " << std::flush;
        usleep(1000000);
    }
    cout << '\n';
}
