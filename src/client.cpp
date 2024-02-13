/*  The client processes that talk to the server
*   in order to correct their temperatures.
*/
#include "../inc/common.h"
using namespace std;

// Supposed to be formatted with a PID for %d
#define CLIENT_QUEUE_NAME   "/jw-coop-processes-client-%d"

// Shuts down the server mq. Called via either signal or manual call.
//  This helps clean up any resources that the message queue used, so that it doesn't strain
//  space in there with an unused mq.
void shutdown_client_mq(int signum) {
    cout << "Shutting down client..." << '\n';

    // Finish use by closing the mq.
    mq_assert((mq_close(qd_client)),
        "Could not close the server mq!");
    mq_assert((mq_close(qd_server)),
        "Could not close the server mq!");
    
    // Successful shutdown via resource cleanup
    cout << "Successfully shut down the client." << '\n';
    exit(0);
}

int main() {
    // Form the client queue name, formatted with the process's PID
    //  Was surprisingly not that convoluted to form. Thanks sprintf!
    char client_queue_name[64];
    sprintf(client_queue_name, CLIENT_QUEUE_NAME, getpid());

    // Create and open the server mq
    //  The client isn't even allowed to open if the server isn't running! That's convenient.
    mq_assert((qd_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY)),
        "Client: mq_open(server) failed - Is the server running?");
    // Create and open the client mq
    mq_assert((qd_client = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)),
        "Client: mq_open(client) failed - what went wrong?");
    
    // After opening, connect the interrupt signal to the shutdown method
    // In case of something like ctrl+c termination
    signal(SIGINT, shutdown_client_mq);

    // Successful open
    cout << "Client and server mq successfully opened." << '\n';
    cout << "Client is listening from queue name: " << client_queue_name << "\n";

    // Send the client's name over to the server
    mq_assert((mq_send (qd_server, client_queue_name , strlen(client_queue_name) + 1, 0)),
        "Client failed to send message to the server.");

    // Wait for a response from the server
    //  Message recieved is the client's temperature, assigned by
    //  the server.
    mq_assert((mq_receive (qd_client, inbuf, MSG_BUFFER_SIZE, NULL)),
        "Client: mq_recieve failed. What went wrong?");

    // Keep listening for the server to say the clients are ready.
    bool ready = false;
    while (!ready) {
        mq_assert((mq_receive (qd_client, inbuf, MSG_BUFFER_SIZE, NULL)),
            "Client: mq_recieve failed. What went wrong?");
        bufstr = str(inbuf);
    }

    // Shutdown the client
    shutdown_client_mq(0);
}
