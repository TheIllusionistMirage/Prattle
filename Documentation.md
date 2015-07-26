Contents of this file
======================

* [Introduction](#introduction)
* [Server-Client Protocol](#server-client-protocol)
* [Server-(Server-Controller) Protocol](#server-server-controller-protocol)

Introduction
-------------

A centralized server hosts the chat, it is responsible for :
* Communication between clients (through packets)
* Handling user login and registration
* Managing the user database.

The clients communicate through the server, i.e. all packets
shared between clients are first received by the server and
then sent to the intended receiver.
The server could be controlled or given commands using the "Server-Controller" using the specific protocol.

So, intuitively, the chat process can be visualized as below:

```

                            ============
                            | CLIENT 2 |
                            ============
                                 |
                                 |
                                 |
                                 |
                                 |
                                 |
  ============               ==========               ============
  | CLIENT 1 | ------------- | SERVER | ------------- | CLIENT 3 |
  ============               ==========               ============
                                 |
                                 |
                                 |
                                 |
                                 |
                                 |
                            ============
                            | CLIENT 4 |
                            ============


```

The entire communication process is made using a set of six rules in the protocol



Server-Client Protocol
-----------------------

(Incomplete, rules are added/modified when a feature is implemented)  
Request format :
```
<Request-type> <client-request-id> <other-stuff>
```
The client-request-id is sent back to the client, it is only specific to the client and couldn't be used by the server.  
Response format:
```
<Response-type> <request-id of corresponding request> <other-stuff>
```
Note : Each word seperated by a space is a different string, thus must be extracted individually from the packet.


**1. Login**

    Packet Type and description        | Packet format
    -----------------------------------|-----------------------------------------------
    1.1 Sent to Server to login        | login \<req id> \<username> \<password>
    1.2 Sent to Client as reply        | login_success \<req id> \<number of friends> \<friend1> \<friend2> ...
    1.3 Sent to Client as reply        | login_failure \<req id> \<server message>


**2. Signup**

    Packet Type                        | Packet format
    -----------------------------------|-----------------------------------------------
    2.1 Sent to Server to signup       | signup \<req id> \<username> \<password>
    2.2 Sent to Client as reply        | signup_success \<req id>
    2.3 Sent to Client as reply        | signup_failure \<req id> \<server message>


**3. Message sending between two (or more) Clients**

    Packet Type                                  | Packet format
    ---------------------------------------------|-----------------------------------------------
    3.1 Sent to Server to send message           | sendmsg \<req id> \<receiver> \<data>
    3.2 Sent to Client(receiver)                 | sendmsg \<sender> \<data>
    3.3 Sent to Client(sender) as reply          | sendmsg_success \<req id>
    3.4 Sent to Client(sender) as reply          | sendmsg_failure \<req id> \<server message>


**4. Searching Database**

    Packet Type                                    | Packet format
    -----------------------------------------------|-----------------------------------------------
    4.1 Sent to Server to search a user            | search_user \<req id> \<name>
    4.2 Sent to Client as reply (found)            | search_user_result \<req id> \<number of matches> \<name1> \<name2> ...
    4.3 Sent to Client as reply (not found)        | search_user_result \<req id> 0
TODO: Limit and/or "paginate" the matches

**5. Adding friends**

    Packet Type                            | Packet format
    ---------------------------------------|-----------------------------------------------
    5.1 Sent to Server                     | add_friend \<req id> \<friendname>
    5.2 Sent to Client as reply            | add_friend_success \<req id> \<friendname>
    5.3 Sent to Client (friend)            | add_new_friend \<friendname>
    5.4 Sent to Client as reply            | add_friend_failure \<req id> \<friendname> \<server message>

**6. Status **

    Packet Type                                               | Packet format
    ----------------------------------------------------------|------------------------------------------------
    6.1 Sent to Client to notify                              | status_online \<friend who came online>
    6.2 Sent to Client to notify                              | status_offline \<friend who went offline>



Server-(Server-Controller) Protocol
-----------------------------------


**Local commands for Server-Controller**

| Command                     | Description
|-----------------------------|---------------------------------------------------------------
| help                        | Print help
| connect \<ip> \<port>       | Connect to Server at IP \<ip> and port \<port>
| connect \<hostname> \<port> | Connect to Server at \<hostname> e.g. "localhost" at port \<port>
| disconnect                  | Disconnect from Server
| exit                        | Disconnect (if connected) and exit

After connecting, a packet is sent to complete the procedure :
```
controller_attach <passphrase>
```
Where \<passphrase> is taken from user.

**Commands sent to the server**

The request sent to the server is the same as the entered command.
For a successful request, the server replies with a packet
```
ack <additional details or queried data>
```


| Command             | Description
|---------------------|---------------------------------------------------------------
| shutdown            | Shutdown the server
| show_logged_users   | Prints the usernames of all logged users
| remove_user \<name> | Removes user \<name> from the DB
| print_stats         | Prints stats about the server (uptime, number of users etc.)  |

Note: Communication between Server-Controller and Server is assumed synchronous