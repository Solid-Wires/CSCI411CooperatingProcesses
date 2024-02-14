#include "../inc/client.h"
using namespace std;

// Overview of the client's main procedure:
//      1.) Send client external temperature
//      2.) Await server response
//          a.) Server responds with central temp - update external temp and repeat
//          b.) Server respond with END - end the main procedure

// This is the calculation for the external temperature of this client.
float ExternalTempCalculation(float centralTemp) {
    return ((clientExtTemp * 3.0) + (2.0 * centralTemp)) / 5.0;
}

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
    cout << "Server sent ready response. All clients are ready." << '\n';
}

// ---------------- MAIN PROCEDURE ----------------
// The client sends the temperature it has over to the server and awaits a response.
//  If the response is a central temperature, update the client's external temperature.
//  If the response is END, then the procedure ends.
void ListenForCentralTempAndUpdateExternalTemp() {
    cout << "Starting client's main procedure..." << '\n';

    // Keep running until you receive a shutdown
    bool shutdown = false;
    while (!shutdown) {
        // Send your temperature to the server.
        sprintf(outbuf, FLOAT_DEC_PLACES_FORMAT, clientExtTemp);
        send(qd_server);

        // Listen for the server's response.
        listen(qd_client);
        // Server's central temperature is expected
        float receivedCentralTemp;
        // Let's see if it's a float
        try {
            receivedCentralTemp = stof(inbuf);
        } catch(...) {
            // Then it's a string. Perhaps the shutdown message?
            string msg = inbuf;
            if (strcmp(inbuf, CLIENT_END_MESSAGE) == 0) { // char[] and str comparisons
                // Procedure can end then.
                shutdown = true;
                continue;
            }
            else {
                // What in the world is this message?
                cout << client_queue_name << " received an unrecognized message..." << '\n';
                cout << "The message received is: " << inbuf << '\n'; 
            }
        }
        // Otherwise, at this point, the message must be the central temperature.
        //  Update the external temperature of this client.
        //      The procedure repeats to send this new temperature again until the server sends
        //      the CLIENT_END_MESSAGE.
        clientExtTemp = ExternalTempCalculation(receivedCentralTemp);
    }

    // Client is ready to shutdown. Although the message only ends this procedure - the actual
    //  client shutdown is handled by Client.cpp
    sprintf(outbuf, FLOAT_DEC_PLACES_FORMAT, clientExtTemp); // Just so I can format the final temp
    cout << "Client confirmed the shutdown message. Ending main procedure." << '\n';
    cout << client_queue_name << "'s final temperature was computed to be " << outbuf << '\n';
}
