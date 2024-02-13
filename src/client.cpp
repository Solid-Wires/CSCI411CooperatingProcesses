/*  The client processes that talk to the server
*   in order to correct their temperatures.
*/
#include "../inc/common.h"
using namespace std;

// This is the client itself in this context here
mqd_t qd_client;

// Supposed to be formatted with a PID for %d
#define CLIENT_QUEUE_NAME   "/jw-coop-processes-client-%d"
char client_queue_name[64];
// This specific client's external temperature. It isn't initialized until
//  the server responds to it.
float clientExtTemp;

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

// Perhaps the longest method name I ever written
//  The client greets the server with its name, then it awaits
//  for the server to provide its temperature information. Once it
//  receives that, it awaits for the server to tell it that it's ready
//  once it receives a sufficient amount of clients to talk to.
void GreetAndAwaitInitiationResponseFromServer() {
    cout << "Greeting server with identity " << client_queue_name << "..." << '\n';

    // Send the client's name over to the server
    mq_assert((mq_send (qd_server, client_queue_name , strlen(client_queue_name) + 1, 0)),
        "Client failed to send message to the server.");

    // Wait for a response from the server
    //  Message recieved is the client's temperature, assigned by
    //  the server.
    mq_assert((mq_receive (qd_client, inbuf, MSG_BUFFER_SIZE, NULL)),
        "Client: mq_recieve failed. What went wrong?");
    // Client's external temperature has been received and initialized!
    clientExtTemp = stof(inbuf);
    cout << "Received external temperature of " << clientExtTemp << " from server." << '\n';

    // Keep listening for the server to say the clients are ready.
    cout << "Waiting for server's ready response..." << '\n';
    bool ready = false;
    while (!ready) {
        mq_assert((mq_receive (qd_client, inbuf, MSG_BUFFER_SIZE, NULL)),
            "Client: mq_recieve failed. What went wrong?");
        // Whenever this fails or not largely depends on what's sent through the inbuf.
        try {
            bool response = stoi(inbuf); // Supposed to be 1 = true
            ready = response;
        } catch(...) {
            // Otherwise, something went wrong. Shut the client down.
            cout << "ERROR: Unexpected response from server. Shutting down." << '\n';
            shutdown_client_mq(0);
        }
    }
    
    cout << "Server sent ready response. All clients are ready. Initiating temp calculation message sequence." << '\n';
}

int main() {
    // Form the client queue name, formatted with the process's PID
    //  Was surprisingly not that convoluted to form. Thanks sprintf!
    sprintf(client_queue_name, CLIENT_QUEUE_NAME, getpid());

    // Create and open the server mq
    //  The client isn't even allowed to open if the server isn't running! That's convenient.
    mq_assert((qd_server = mq_open(SERVER_QUEUE_NAME, O_WRONLY)),
        "Client: mq_open(server) failed - Is the server running?");
    // Create and open the client mq
    mq_assert((qd_client = mq_open(client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)),
        "Client: mq_open(client) failed - what went wrong?");
    
    // After opening, connect the interrupt signal to the shutdown method
    // In case of something like ctrl+c termination
    signal(SIGINT, shutdown_client_mq);

    // Successful open
    cout << "Client and server mq successfully opened." << '\n';

    // First of all, let the server know that it exists and grab its temperature from it.
    GreetAndAwaitInitiationResponseFromServer();

    // Shutdown the client
    shutdown_client_mq(0);
}
