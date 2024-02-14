/*  The client processes that talk to the server
*   in order to correct their temperatures.
*/
#include "../inc/client.h"
using namespace std;

// This is the client itself in this context here
mqd_t qd_client;

// Supposed to be formatted with a PID for %d
char client_queue_name[MAX_CLIENT_NAME_SIZE];
// This specific client's external temperature. It isn't initialized until
//  the server responds to it.
int clientExtTemp;

// Shuts down the client mq. Called via either interrupt signal or manual call.
//  This helps clean up any resources that the message queue used, so that it doesn't strain
//  space in there with an unused mq.
void ShutdownMQ(int signum) {
    cout << "Shutting down client..." << '\n';
    shuttingDown = true;

    // Finish use by closing the mq.
    assert((mq_close(qd_client)),
        processName + ": Could not close the client mq!");
    assert((mq_close(qd_server)),
        processName + ": Could not close the server mq!");
    // Unlink the client (delete the message queue).
    //  Since it's no longer needed in the filesystem.
    assert((mq_unlink(client_queue_name)),
        processName + ": Could not delete the client mq! Did it ever exist?");
    
    // Successful shutdown via resource cleanup
    cout << "Successfully shut down client" << processName << '\n';
    exit(0);
}

int main() {
    // Get the pid
    pid = getpid();

    // Form the client queue name, formatted with the process's PID
    //  Was surprisingly not that convoluted to form. Thanks sprintf!
    sprintf(client_queue_name, CLIENT_QUEUE_NAME, pid);
    processName = client_queue_name;

    // Introduce yourself
    introduce();

    // Create and open the server mq
    //  The client isn't even allowed to open if the server isn't running! That's convenient.
    assert((qd_server = mq_open(SERVER_QUEUE_NAME, O_WRONLY)),
        processName + ": mq_open(server) failed - Is the server running?");
    // Create and open the client mq
    assert((qd_client = mq_open(client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)),
        processName + ": mq_open(client) failed - what went wrong?");
    
    // After opening, connect the interrupt signal to the shutdown method
    // In case of something like ctrl+c termination
    signal(SIGINT, ShutdownMQ);

    // Successful open
    cout << "Client and server mq successfully opened." << '\n';

    // First of all, let the server know that it exists and grab its temperature from it.
    GreetAndAwaitInitiationResponseFromServer();
    // It waits for a bit for the server to send the ready signal.

    // After that, perform the client's main procedure.
    ListenForCentralTempAndUpdateExternalTemp();

    // Shutdown the client This is a good shutdown.
    cout << '\n';
    ShutdownMQ(0);
}
