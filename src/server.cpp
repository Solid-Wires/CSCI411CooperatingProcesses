/*  The server process that talks to the clients
*   in order to correct their temperatures.
*/
#include "../inc/common.h"
using namespace std;

// All known clients that greeted the server.
vector<string> clients;
// Each client gets their initial temperature based on the order in when they greet the server.
//  From earliest greeter to latest greeter.
//  NOTE: This is exclusive to the server instance - the clients don't know their temperatures yet!
float initialClientTemps[4] = {100.0, 22.0, 50.0, 40.0};

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

// The first procedure that the server should do is wait for all of the clients to
//  shake its hand. It records all of the clients that the server gets to know and sends
//  them back their temperatures on a first-come-first-served basis.
void WaitForClients() {
    // Loop while there are less than 4 clients known.
    while (clients.size() < 4) {
        // Recieve a greeting from a client.
        mq_assert((mq_receive(qd_server, inbuf, MSG_BUFFER_SIZE, NULL)),
            "Server: mq_receive failed. What went wrong?");
        // New client! Give them their temperature!
        clients.push_back(inbuf);
        int clientIdx = clients.size() - 1;
        float clientInitialTemp = initialClientTemps[clientIdx];
        cout << "Welcome! Shook hands with a new client named " << clients.back() << '\n';

        //Send them their temperature after the hand shake.
        cout << "\t Sending initial temperature: " << clientInitialTemp << "" << '\n';
        sprintf(outbuf, "%.1f", clientInitialTemp);
        mq_assert((mq_send(qd_client, outbuf, MSG_BUFFER_SIZE, NULL)),
            "Server: mq_receive failed. What went wrong?");
    }

    // Server is now ready to get started.
    cout << "Server now has all of the clients it was expecting. Sending ready signal." << '\n';
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

    // Wait for all clients to connect to the server and get their temperatures.
    WaitForClients();
    
    // Shutdown the server.
    shutdown_server_mq(0);
}
