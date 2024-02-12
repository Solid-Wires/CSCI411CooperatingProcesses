/*  The client processes that talk to the server
*   in order to correct their temperatures.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <cstring>
#include <iostream>
#include "../inc/common.h"
using namespace std;

int main() {
    // Queue descriptors
    mqd_t qd_server, qd_client;

    cout << SERVER_QUEUE_NAME << '\n';
}
