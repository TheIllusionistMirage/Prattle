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
Basic packet format :
```
<Request type> <sender> <receiver> <other details>
```
Note : Each word seperated by a space is a different string, thus must be extracted individually from the packet.


**1. Login**

    Packet Type and description        | Packet format
    -----------------------------------|-----------------------------------------------
    1.1 Sent to Server to login        | login \<username> \<password>
    1.2 Sent to Client as reply        | login_success \<number of friends> \<friend1> \<friend2> ...
    1.3 Sent to Client as reply        | login_failure \<server message>


**2. Signup**

    Packet Type                        | Packet format
    -----------------------------------|-----------------------------------------------
    2.1 Sent to Server to signup       | signup \<username> server \<password>
    2.2 Sent to Client as reply        | signup_success
    2.3 Sent to Client as reply        | signup_failure \<server message>


**3. Message sending between two (or more) Clients**

    Packet Type                                  | Packet format
    ---------------------------------------------|-----------------------------------------------
    3.1 Sent to Server to send message           | sendmsg \<receiver> \<data>
    3.2 Sent to Client(receiver)                 | sendmsg server \<sender> \<data>
    3.3 Sent to Client(sender) as reply          | sendmsg_success \<receiver> \<djb2 hash of the message>
    3.4 Sent to Client(sender) as reply          | sendmsg_failure \<receiver> \<djb2 hash of the message> \<server message>


**4. Searching Database**

    Packet Type                                    | Packet format
    -----------------------------------------------|-----------------------------------------------
    4.1 Sent to Server to search a user            | search_user \<name>
    4.2 Sent to Client as reply (found)            | search_user_result \<number of matches> \<name1> \<name2> ...
    4.3 Sent to Client as reply (not found)        | search_user_result 0
TODO: Limit and/or "paginate" the matches

**5. Adding friends**

    Packet Type                            | Packet format
    ---------------------------------------|-----------------------------------------------
    5.1 Sent to Server                     | add_friend \<friendname>
    5.2 Sent to Client as reply            | add_friend_success \<friendname>
    5.3 Sent to Client as reply            | add_friend_failure \<friendname> \<server message>

**6. Status **

    Packet Type                                               | Packet format
    ----------------------------------------------------------|------------------------------------------------
    6.1 Sent to Client to notify                              | status_online \<friend who came online> ...
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

