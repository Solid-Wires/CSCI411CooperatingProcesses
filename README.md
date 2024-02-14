# CSCI 411 Cooperating Processes Lab

This assignment is having one server process listen to 4 client processes in order to stabilize all
of their temperatures.

# Assumptions

* Temperatures are integers (Used to be in floats, but I changed my mind due to the wording "must exactly match")
* Each message sent displays the message queue (& potentially with the PID related) and the message being sent. The clients don't know the PID of the server, and I would've fixed that but I ran out of time.
* Each message is sent and received one at a time. I tried to use structs, but they didn't work out in my experience. So I kept the message procedures simple.

# How to run
Simply run make to create the executables. You should see bin and obj directories created in the workspace.

bin/server runs the server process, and bin/client runs the client process.

The server process must run first before the client processes. After the server is up and running, you must run 4 client processes in separate bash instances. The clients will automatically communicate with the server when you execute them.

After all 4 client processes are opened, the server begins a countdown and sends a ready signal for all clients to send their temperatures. The procedures described are ran until temperature stability is achieved across all clients. After finishing, the server sends an end message to all clients and both the client and server makes sure to clean up all mq resources that they used.

NOTE: If at any point that either executable fails, then it tries to clean up queues anyways and shut down. You can also interrupt any process at any time and it will clear its resources.

# Analysis

From what I've observed, the clients and server will stabilize at a temperature unit of 40 given these initial external temperatures and formulas.
