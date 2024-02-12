/*
 * server.c: Server program
 *           to demonstrate interprocess commnuication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <cstring>
#include <iostream>

// DEFINE THE SERVER NAME AN DEFAULT VALUES FOR THE MESSAGE QUEUE
#define SERVER_QUEUE_NAME   "/coop-processes-server"
#define QUEUE_PERMISSIONS 0660  // like chmod values, user and owner can read and write to queue
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10   // leave some extra space after message

using namespace std;
/****************************************************************************
START OF MAIN PROCEDURE
This server creates a message queue and waits for a message requesting a token
The message received also has the name of the client mailbox.  The server uses
that name to reply.
*****************************************************************************/

int main ()
{
    // queue descriptors
    mqd_t qd_server, qd_client;
    
	// Build message queue attribute structure passed to the mq open
    struct mq_attr attr;

		attr.mq_flags = 0;
		attr.mq_maxmsg = MAX_MESSAGES;
		attr.mq_msgsize = MAX_MSG_SIZE;
		attr.mq_curmsgs = 0;

	// Declare (create) the buffers to hold message received and sent
    char in_buffer [MSG_BUFFER_SIZE];
    char out_buffer [MSG_BUFFER_SIZE];
	
	// Initialize the token to be given to client
	int token_number = 1; 
	
	// Open and create the server message queue
	cout << "Server: Hello, World!\n";
	
    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        cerr << "Server: mq_open (server)";
        exit (1);
    }
	
	// Loop forever waiting for a request for a token
    while (1) {
		
		// RECEIVE THE MESSAGE FROM SERVER MAILBOX
        // get the oldest message with highest priority
		// Note: this synchronous receive will wait until a message is in the queue
        if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE - 2, NULL) == -1) {
            cerr << "Server: mq_receive";
            exit (1);
        }

        cout << "Server: message received.\n";

        // SEND REPLY MESSAGE TO CLIENT'S MAILBOX
		// The message received, contains the name of the client's mailbox.
		// That name is saved in the input buffer.

        // Open the client's mailbox
        //  (this was checking against 1... Typo?)
        //  YES IT WAS. THIS WAS THE REASON WHY IT WASN'T SENDING.
        cout << in_buffer << '\n';
        if ((qd_client = mq_open ("/coop-processes-client-test", O_WRONLY)) == -1) {
            cerr << "Server: Not able to open client queue";
            continue;
        }

        // The following copies an integer (token) into the c-string that is the output buffer
		sprintf(out_buffer, "%d", token_number);

        // Send the message to the client's mailbox/mailqueue
		if (mq_send (qd_client, out_buffer, strlen (out_buffer), 0) == -1) {
            cerr << "Server: Not able to send message to client";
            continue;
        }
		
		// Server outputs the successful reply to the client and increments the token for the next request
        cout << "Server: sent token " << token_number << " to client: " << in_buffer << endl;
        token_number++;
    }
}