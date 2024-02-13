#include "../inc/common.h"
using namespace std;

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
