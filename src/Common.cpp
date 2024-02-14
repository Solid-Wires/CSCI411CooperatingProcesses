#include "../inc/common.h"
using namespace std;

// Both instances expect Queue Descriptors for the server
//  However, the context for qd_client from both instances are completely different
mqd_t qd_server;

// The pid and custom name of the instance, stored.
//  processName is mainly used for debugging
int pid;
std::string processName;
bool shuttingDown = false;

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
//  Which means it failed. Prints off why and try to shut the process down gracefully.
// You may not always need the program to shutdown after an assertion, so you can
//  prevent that by setting the shutdown arg to false.
void assert(int code, string why, bool shutdown) {
    if (code == -1) {
        cerr << why << '\n';
        // Try to shut down gracefully, if it isn't already shutting down
        //  Otherwise, do report the why in the assertion
        if (shutdown && !shuttingDown) {
            ShutdownMQ(0);
        }
    }
}
// Overloaded assert, making the shutdown bool optional
void assert(int code, string why) {
    assert(code, why, true);
}

// Sends a message from whatever is in outbuf to a given descriptor.
void send(mqd_t mq_desc, string toWho) {
    // Part of the assignment's reqs is to report every send
    if (REPORT_SEND) {
        cout << toWho << ": " << outbuf << '\n';
    }
    assert((mq_send(mq_desc, outbuf, strlen(outbuf) + 1, 0)),
        processName + ": failed to send a message.");
}
// Listens for a message from a descriptor and puts it into inbuf.
//  Note that mq_receive halts the program at wherever it is until it receives a message
void listen(mqd_t mq_desc) {
    assert((mq_receive (mq_desc, inbuf, MSG_BUFFER_SIZE, NULL)),
        processName + ": mq_recieve failed.");
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
