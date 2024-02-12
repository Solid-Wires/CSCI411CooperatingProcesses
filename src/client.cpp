/*  The client processes that talk to the server
*   in order to correct their temperatures.
*/
#include "../inc/common.h"
using namespace std;

// Supposed to be formatted with a PID for %d
#define CLIENT_QUEUE_NAME   "/jw-coop-processes-client-%d"

int main() {
    // Queue descriptors
    mqd_t qd_server, qd_client;

    // Form the client queue name, formatted with the process's PID
    char client_queue_name[64];
    sprintf(client_queue_name, CLIENT_QUEUE_NAME.c_str(), getpid());

    cout << client_queue_name << '\n';
}
