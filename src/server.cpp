/*  The server process that talks to the clients
*   in order to correct their temperatures.
*/
#include "../inc/common.h"
using namespace std;

int main() {

    // Open and create the server message queue
    cout << "Opening server mq..." << '\n';
    mq_assert((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)),
        "Server: mq_open(server) had a problem.");
    
    // Successful open
    cout << "Server mq successfully opened." << '\n';
    cout << "Server is listening from queue name: " << SERVER_QUEUE_NAME << "\n";

    // Server keeps listening until it is finished.
    bool finished = true;
    while (!finished) {
        // Recieve any message from the server's mailbox.
        mq_assert((mq_receive(qd_server, inbuf, MSG_BUFFER_SIZE, NULL)),
            "Server: mq_receive had a problem.");
    }

    cout << "Shutting down server mq..." << '\n';

    // Finish use by closing the mq.
    mq_assert((mq_close(qd_server)),
        "Could not close the server mq!");
    // Unlink the server (delete the message queue).
    //  Since it's no longer needed in the filesystem.
    mq_assert((mq_unlink(SERVER_QUEUE_NAME)),
        "Could not delete the server mq!");
    
    // Successful shutdown via resource cleanup
    cout << "Successfully shut down the server mq." << '\n';
}
