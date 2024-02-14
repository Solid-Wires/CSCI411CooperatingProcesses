#include "../inc/server.h"
using namespace std;

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
            tempsReceivedSum += temperature;
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