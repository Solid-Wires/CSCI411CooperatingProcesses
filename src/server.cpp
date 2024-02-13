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

// The central server temp. It is initialized to 0 right now because it isn't known until it
//  receives all 4 of the clients' temps.
float serverCentralTemp = 0;

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

// Send the outbuf message to all clients.
void SendToAllClients() {
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
        listen(qd_server);

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
    SendToAllClients();
    cout << "Ready signal sent." << '\n';
}

// Keep the server on standby, waiting for all of the clients' responses until it finds
//  out that all 4 responses from the clients are stable temperatures.
void RunUntilClientsAreStable() {
    bool stabilized = false;
    while (!stabilized) {
        // All temperatures received are added into here
        float tempsReceivedSum = 0;
        // The conditional procedure for whenever or not the clients are stable.
        //  Let's assume that they are stable - then disprove or approve of this assumption.
        float lastTempReceived;
        int code = 0; // Check off of a condition code: 0 means initial, 1 means stable, 2 means unstable

        // Listen from itself from all clients
        //  Sum up each temperature received
        //  And also perform stability checking
        for (string client : clients) {
            listen(qd_server);
            float temperature = stof(inbuf);
            switch(code) {
                // Skip this entirely - it was figured out before that the system is unstable.
                case 2: break;
                // Initial - we don't know for sure yet. Set the initial last temp received.
                case 0:
                    lastTempReceived = temperature;
                    code = 1;
                    break;
                // Assumption checking - The last temperature comparison was stable, but what about this one?
                case 1:
                    if (lastTempReceived == temperature) {
                        // Don't change the code. Make this the last temperature received.
                        //  This doesn't mean that the clients are stable yet, though.
                        lastTempReceived = temperature;
                    }
                    else {
                        // These two temps are not the same. The clients are unstable.
                        //  Change the code to 2 - unstable.
                        code = 2;
                    }
                    break;
            }
            tempsReceivedSum += tempsReceived.back();
        }

        // Calculate the new server central temperature.
        serverCentralTemp = ((2 * serverCentralTemp) + tempsReceivedSum) / 6;

        // Check against code received.
        if (code > 1) {
            // The clients are, in whatever capacity described, still unstable. Send them a new central temperature.
            sprintf(outbuf, "%.1f", serverCentralTemp);
            SendToAllClients();
        }
        else if (code == 1) {
            // The clients are fully synchronized. Stability was achieved.
            stabilized = true;
        }
        else { // Bad code
            // The code never changed from initial?
            if (code == 0) {
                cout << "ERR: Never changed condition code from 0, so no comparisons were made." << '\n';
            }
            // Did I goof up and write an undefined code?
            else {
                cout << "ERR: Undefined condition code." << '\n';
            }
            // Anyways, shut down the server and send shutdown message to all clients.
            sprintf(outbuf, "%s", CLIENT_END_MESSAGE);
            SendToAllClients();
            ShutdownMQ(1);
        }
    }

    // If we've left the loop, then all clients have successfully been synchronized. The temperatures are now stable.
    cout << "All clients have successfully had their temperature stabilized." << '\n';
    cout << "The final central temperature was computed to be: " << serverCentralTemp << '\n';
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
    signal(SIGINT, ShutdownMQ);
    
    // Successful open
    cout << "Server mq successfully opened." << '\n';
    cout << "Server is listening from queue name: " << SERVER_QUEUE_NAME << "\n";

    // Wait for all clients to connect to the server and get their temperatures.
    WaitForClients();

    // All clients have received their temperatures and now the server recognizes them.
    //  Now the server will listen and respond with temperature calculations from all clients.
    RunUntilClientsAreStable();
    
    // Shutdown the server. This is a good shutdown.
    cout << '\n';
    ShutdownMQ(0);
}
