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

// States the program's identity
void introduce() {
    cout << "This process is: " << processName << '\n';
    cout << "The PID is: " << pid << '\n';
}
// Shorthand for the if-statement check when the mq result == -1.
//  Which means it failed. Prints off why and closes the application.
void assert(int code, string why) {
    if (code == -1) {
        cerr << why << '\n';
        exit(1);
    }
}

// Sends a message from whatever is in outbuf to a given descriptor.
void send(mqd_t mq_desc) {
    // Part of the assignment's reqs is to report every send
    if (REPORT_SEND) {
        cout << pid << ": " << outbuf;
    }
    assert((mq_send(mq_desc, outbuf, strlen(outbuf) + 1, 0)),
        processName + " failed to send a message to " + client);
}

// Countdown to READY_WAIT_SEC
void countdown() {
    int maxWaitSecs = READY_WAIT_SEC;
    for (int i = READY_WAIT_SEC; i > 0; i--) {
        cout << i << "... " << std::flush;
        usleep(1000000);
    }
    cout << '\n';
}
