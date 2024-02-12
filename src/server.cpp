/*  The server process that talks to the clients
*   in order to correct their temperatures.
*/
#include "../inc/common.h"
using namespace std;

int main() {

    // Open and create the server message queue
    cout << "Opening server mq..." << '\n';
    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        cerr << "Server: mq_open (server)";
        exit (1);
    }
    cout << "Server mq successfully opened." << '\n';
    cout << "Server is listening from queue name: " << SERVER_QUEUE_NAME << "\n";

    // bool all_synced = false;
    // while (!all_synced) {

    // }

    cout << "Shutting down server mq..." << '\n';

    // Unlink the server (delete the message queue).
    if (mq_unlink(SERVER_QUEUE_NAME) != -1) {
        cerr << "Could not delete the server mq!" << '\n';
        exit(1);
    }
    cout << "Successfully shut down the server mq." << '\n';
}
