/*  The server process that talks to the clients
*   in order to correct their temperatures.
*/
#include "../inc/common.h"
using namespace std;

// The central server temp. It is not initialized right now because it isn't known until it
//  receives all 4 of the clients' temps.
float serverCentralTemp;

// Shuts down the server mq. Called via either signal or manual call.
//  This helps clean up any resources that the message queue used, so that it doesn't strain
//  space in there with an unused mq.
void shutdown_server_mq(int signum) {
    cout << "Shutting down server..." << '\n';

    // Finish use by closing the mq.
    mq_assert((mq_close(qd_server)),
        "Could not close the server mq! Was it ever opened?");
    // Unlink the server (delete the message queue).
    //  Since it's no longer needed in the filesystem.
    mq_assert((mq_unlink(SERVER_QUEUE_NAME)),
        "Could not delete the server mq! Did it ever exist?");
    
    // Successful shutdown via resource cleanup
    cout << "Successfully shut down the server." << '\n';
    exit(0);
}

int main() {

    // Open and create the server message queue
    cout << "Opening server mq..." << '\n';
    mq_assert((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)),
        "Server: mq_open(server) failed. What went wrong?");

    // After opening, connect the interrupt signal to the shutdown method
    // In case of something like ctrl+c termination
    signal(SIGINT, shutdown_server_mq);
    
    // Successful open
    cout << "Server mq successfully opened." << '\n';
    cout << "Server is listening from queue name: " << SERVER_QUEUE_NAME << "\n";

    // Server keeps running until it is finished.
    bool finished = false;
    while (!finished) {
        // Recieve any message from the server's mailbox.
        mq_assert((mq_receive(qd_server, inbuf, MSG_BUFFER_SIZE, NULL)),
            "Server: mq_receive had a problem.");
        
        cout << "Server received message" << '\n';

        // TODO: What do I do with the buffer?
    }
    
    // Shutdown the server.
    shutdown_server_mq(0);
}
