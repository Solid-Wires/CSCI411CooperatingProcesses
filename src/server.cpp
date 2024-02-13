/*  The server process that talks to the clients
*   in order to correct their temperatures.
*/
#include "../inc/common.h"
using namespace std;

// All known clients that greeted the server.
vector<string> clients;
map<string, mqd_t> openClients;
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
void ShutdownServerMQ(int signum) {
    cout << "Shutting down server..." << '\n';

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
            processName + ": Could not close client mq " + openClients[client] + "!");
    }

    // Successful shutdown via resource cleanup
    cout << "Successfully shut down " << processName << '\n';
    exit(0);
}

// Send the outbuf message to all clients.
void sendToAllClients() {
    for (string client : clients) {
        send(openClients[client]);
    }
}

// The first procedure that the server should do is wait for all of the clients to
//  shake its hand. It records all of the clients that the server gets to know and sends
//  them back their temperatures on a first-come-first-served basis.
void WaitForClients() {
    cout << "Server needs exactly 4 clients to proceed. You may now execute clients." << '\n';

    // Loop while there are less than 4 clients known.
    while (clients.size() < 4) {
        // Recieve a greeting from a client.
        listen(qd_server)

        // New client! Open up the descriptor and give them their temperature!
        clients.push_back(inbuf);
        assert((openClients[clients.back()] = mq_open(clients.back().c_str(), O_WRONLY)),
            "Server: mq_open(client) failed - what went wrong?");
        mqd_t qd_client = openClients[clients.back()];
        int clientIdx = clients.size() - 1;
        float clientInitialTemp = initialClientTemps[clientIdx];
        cout << "Welcome! Shook hands with a new client named " << clients.back() << '\n';

        //Send them their temperature after the hand shake.
        cout << "\t Sending initial temperature: " << clientInitialTemp << "" << '\n';
        sprintf(outbuf, "%.1f", clientInitialTemp);
        send(qd_client);
    }

    // Server is now ready to get started.
    cout << "Server now has all of the clients it was expecting." << '\n';
    cout << "Sending ready signal in " << '\n';
    countdown();
    // Send ready signal now
    sprintf(outbuf, "%d", true);
    sendToAllClients();
}

int main() {
    // Get the pid and set name
    pid = getpid();
    processName = SERVER_QUEUE_NAME;
    // Introduce yourself
    introduce();

    // Open and create the server message queue
    cout << "Opening server mq..." << '\n';
    assert((qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)),
        "Server: mq_open(server) failed. What went wrong?");

    // After opening, connect the interrupt signal to the shutdown method
    // In case of something like ctrl+c termination
    signal(SIGINT, ShutdownServerMQ);
    
    // Successful open
    cout << "Server mq successfully opened." << '\n';
    cout << "Server is listening from queue name: " << SERVER_QUEUE_NAME << "\n";

    // Wait for all clients to connect to the server and get their temperatures.
    WaitForClients();

    // All clients have received their temperatures and now the server recognizes them.
    //  
    
    // Shutdown the server.
    ShutdownServerMQ(0);
}
