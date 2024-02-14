/*  The server process that talks to the clients
*   in order to correct their temperatures.
*/
#include "../inc/server.h"
using namespace std;

// All known clients that greeted the server.
vector<string> clients;
map<string, mqd_t> openClients;

// Each client gets their initial temperature based on the order in when they greet the server.
//  From earliest greeter to latest greeter.
//  NOTE: This is exclusive to the server instance - the clients don't know their temperatures yet!
// If EXPECTED_CLIENTS is changed, you must add or remove new initialized temperatures here!
int initialClientTemps[EXPECTED_CLIENTS] = {100, 22, 50, 40};

// The central server temp. It is initialized to 0 right now because it isn't known until it
//  receives all 4 of the clients' temps.
int serverCentralTemp = 0;

// Shuts down the server mq. Called via either interrupt signal or manual call.
//  This helps clean up any resources that the message queue used, so that it doesn't strain
//  space in there with an unused mq.
void ShutdownMQ(int signum) {
    cout << "Shutting down server..." << '\n';
    shuttingDown = true;

    // Finish use by closing the mq.
    assert((mq_close(qd_server)),
        processName + ": Could not close the server mq!");
    // Unlink the server (delete the message queue).
    //  Since it's no longer needed in the filesystem.
    assert((mq_unlink(SERVER_QUEUE_NAME)),
        processName + ": Could not delete the server mq!");
    // Try to close every connection to the clients.
    //  Each client handles their own mq's deletion
    for (string client : clients) {
        assert((mq_close(openClients[client])),
            processName + ": Could not close client mq " + client + "!");
    }

    // Successful shutdown via resource cleanup
    cout << "Successfully shut down " << processName << '\n';
    exit(0);
}

int main() {
    // Get the pid and set name
    pid = getpid();
    sprintf(processName, SERVER_QUEUE_NAME, pid);
    serverName = processName;
    // Introduce yourself
    introduce();

    // Open and create the server message queue
    cout << "Opening server mq..." << '\n';
    assert((qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)),
        "Server: mq_open(server) failed. What went wrong?");

    // After opening, connect the interrupt signal to the shutdown method
    // In case of something like ctrl+c termination
    signal(SIGINT, ShutdownMQ);
    
    // Successful open
    cout << "Server mq successfully opened." << '\n';
    cout << "Server is listening from queue name: " << SERVER_QUEUE_NAME << "\n";

    // Wait for all clients to connect to the server and get their temperatures.
    WaitForClients();

    // All clients have received their temperatures and now the server recognizes them.
    //  Now the server will listen and respond with temperature calculations from all clients.
    RunUntilClientsAreStable();

    // The clients are allowed to shutdown now.
    ShutdownClients();
    
    // Shutdown the server. This is a good shutdown.
    cout << '\n';
    ShutdownMQ(0);
}
