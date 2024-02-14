#include "..inc/client.h"
using namespace std;

// The client greets the server with its name, then it awaits
//  for the server to provide its temperature information. Once it
//  receives that, it awaits for the server to tell it that it's ready
//  once it receives a sufficient amount of clients to talk to.
void GreetAndAwaitInitiationResponseFromServer() {
    cout << "Greeting server with identity " << client_queue_name << "..." << '\n';

    // Send the client's name over to the server
    sprintf(outbuf, "%s", client_queue_name);
    send(qd_server);

    // Wait for a response from the server
    //  Message recieved is the client's temperature, assigned by
    //  the server.
    listen(qd_client);
    // Client's external temperature has been received and initialized!
    clientExtTemp = stof(inbuf);
    cout << "Received external temperature of " << clientExtTemp << " from server." << '\n';

    // Keep listening for the server to say the clients are ready.
    cout << "Waiting for server's ready response..." << '\n';
    bool ready = false;
    while (!ready) {
        listen(qd_client);
        // Whenever this fails or not largely depends on what's sent through the inbuf.
        try {
            bool response = stoi(inbuf); // Supposed to be 1 = true
            ready = response;
        } catch(...) {
            // Otherwise, something went wrong. Shut the client down.
            cout << "ERROR: Unexpected response from server. Shutting down." << '\n';
            ShutdownMQ(0);
        }
    }
    
    cout << "Server sent ready response. All clients are ready. Initiating temp calculation message sequence." << '\n';
}
