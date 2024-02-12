/*
 * Adapted from  Softpryaog 
  *    https://www.softprayog.in/programming/interprocess-communication-using-posix-message-queues-in-linux
  * by MA Doman 2018
 * client.c: Client program
 *           to demonstrate interprocess communication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <cstring>
#include <iostream>

// DEFINE THE SERVER NAME AN DEFAULT VALUES FOR THE MESSAGE QUEUE
#define SERVER_QUEUE_NAME   "/server"
#define QUEUE_PERMISSIONS 0664  // like chmod values, user and owner can read and write to queue
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

    // create the client queue for receiving messages from server
	// use the client PID to help differentiate it from other queues with similar names
	// the queue name must be a null-terminated c-string.
	// strcpy makes that happen
    char client_queue_name [64];
//	string  str_client_queue_name = "/client-" + to_string(getpid ()) + "\\0'";
	string  str_client_queue_name = "/client-" + std::to_string(getpid ());
	strcpy(client_queue_name, str_client_queue_name.c_str());
    
	// Build message queue attribute structure passed to the mq open
    struct mq_attr attr;
		attr.mq_flags = 0;
		attr.mq_maxmsg = MAX_MESSAGES;
		attr.mq_msgsize = MAX_MSG_SIZE;
		attr.mq_curmsgs = 0;

	char in_buffer [MSG_BUFFER_SIZE];   // Build input buffer
	char temp_buf [10];	                // to hold result of input
	
	// Create and open client message queue
    if ((qd_client = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        cerr<<"Client: mq_open (client)" ;
        exit (1);
    }
	
	// Open server message queue. The name is shared. It is opend write and read only
    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
         cerr<<"Client: mq_open (server)";
        exit (1);
    }


// Loop while the enter key is pressed after the prompt to:
    printf ("Ask for a token (Press <ENTER>): ");
    while (fgets (temp_buf, 2, stdin)) {

        // Send message to server
		//  Data sent is the client's message queue name
        if (mq_send (qd_server, client_queue_name , strlen(client_queue_name), 0) == -1) {
             cerr<<"Client: Not able to send message to server";
            continue;
        }

        // Receive response from server
		// Message received is the token
        if (mq_receive (qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
             cerr<<"Client: mq_receive";
            exit (1);
        }
        // display token received from server
        cout << "Client: Token received from server: " <<  in_buffer << endl;

        cout << endl << "Ask for a token (Press ): "<< endl;
    }

   
	// Close this message queue
    if (mq_close (qd_client) == -1) {
        cerr<<"Client: mq_close";
        exit (1);
    }

	// Unlink this message queue
    if (mq_unlink (client_queue_name ) == -1) {
         cerr<<"Client: mq_unlink";
        exit (1);
    }
    cout << "Client: bye\n";

    exit (0);
}