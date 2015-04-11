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
Request types are case insensitive  


**1. Login**

    Packet Type                        | Packet format
    -----------------------------------|-----------------------------------------------
    1.1 Packet sent to Server          | LOGIN username SERVER password
    1.2 Packet sent to Client as reply | LOGIN-SUCCESS SERVER username user-details
    1.3 Packet sent to Client as reply | LOGIN-FAILURE SERVER username details


**2. Signup**

    Packet Type                        | Packet format
    -----------------------------------|-----------------------------------------------
    2.1 Packet sent to Server          | SIGNUP username SERVER password
    2.2 Packet sent to Client as reply | SIGNUP-SUCCESS SERVER username
    2.3 Packet sent to Client as reply | SIGNUP-FAILURE SERVER username details


**3. Packet exchange between two (or more) Clients**

    Packet Type                                  | Packet format
    ---------------------------------------------|-----------------------------------------------
    3.1 Packet sent to Server                    | SENDMSG sender receiver data
    3.2 Packet sent to Client(receiver) as reply | SENDMSG SERVER sender receiver data
    3.3 Packet sent to Client(sender) as reply   | SENDMSG-SUCCESS SERVER sender
    3.4 Packet sent to Client(sender) as reply   | SENDMSG-FAILURE SERVER sender details


**4. Searching Database**

    Packet Type                                    | Packet format
    -----------------------------------------------|-----------------------------------------------
    4.1 Packet sent to Server                      | SEARCH-USER username SEVER name
    4.2 Packet sent to Client as reply (found)     | SEARCH-USER-RESULT SERVER username name
    4.3 Packet sent to Client as reply (not found) | SEARCH-USER-RESULT SERVER username

**5. Adding friends**

    Packet Type                            | Packet format
    ---------------------------------------|-----------------------------------------------
    5.1 Packet sent to Server              | ADD-FRIEND username SERVER friendname
    5.2 Packet sent to Client as reply     | ADD-FRIEND-SUCCESS SERVER username
    5.3 Packet sent to Client as reply     | ADD-FRIEND-FAILURE SERVER username details

**6. Notification**

    Packet Type                                               | Packet format
    ----------------------------------------------------------|-----------------------------------------------
    6.1.1 Packet sent to Client for a friend connecting       | NOTIF-LOGIN  SERVER username friendname
    6.1.2 Packet sent to Client for a friend disconnecting    | NOTIF-LOGOUT SERVER username friendname
    6.2.3 Packet sent to Server for currently online friends  | NOTIF-ONLINE username SERVER
    6.2.4 Packet sent to Client as reply                      | NOTIF-ONLINE SERVER username onlineFriends

Server-(Server-Controller) Protocol
-----------------------------------


**Local commands for Server-Controller**  

| Command              | Description                                                   |
|----------------------|---------------------------------------------------------------|
| help                 | Print help                                                    |
| connect <ip> <port>  | Connect to Server at IP <ip> and port <port>                  |
| disconnect           | Disconnect from Server                                        |
| exit                 | Disconnect (if connected) and exit                            |

After connecting, a packet is sent to complete the procedure :  
```
controller_attach <passphrase>
``` 
Where <passphrase> is taken from user.  

**Commands sent to the server**

The request sent to the server is the same as the entered command.  
For a successful request, the server replies with a packet  
```
ack <additional details or queried data>
```


| Command            | Description                                                   |
|--------------------|---------------------------------------------------------------|
| shutdown           | Shutdown the server                                           |
| show_logged_users  | Prints the usernames of all logged users                      |
| remove_user <name> | Removes user <name> from the DB                               |
| print_stats        | Prints stats about the server (uptime, number of users etc.)  |

