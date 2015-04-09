A centralized server hosts the chat, it is responsible for :  
* Communication between clients (through packets)
* Handling user login and registration
* Managing the user database.

The clients communicate through the server, i.e. all packets
shared between clients are first received by the server and
then sent to the intended receiver.

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

The entire communication process is made using a set of five rules in the protocol



Protocol
=========



1. Login
----------

    Packet Type                        | Packet format
    -----------------------------------|-----------------------------------------------
    1.1 Packet sent to Server          | LOGIN username SERVER password
    1.2 Packet sent to Client as reply | LOGIN-SUCCESS SERVER username user-details
    1.3 Packet sent to Client as reply | LOGIN-FAILURE SERVER username details


2. Signup Protocols
-----------------------------

    Packet Type                        | Packet format
    -----------------------------------|-----------------------------------------------
    2.1 Packet sent to Server          | SIGNUP username SERVER password
    2.2 Packet sent to Client as reply | SIGNUP-SUCCESS SERVER username
    2.3 Packet sent to Client as reply | SIGNUP-FAILURE SERVER username details


3. Packet exchange between two (or more) Clients Protocols
----------------------------------------------------------

    Packet Type                                  | Packet format
    ---------------------------------------------|-----------------------------------------------
    3.1 Packet sent to Server                    | SENDMSG sender receiver data
    3.2 Packet sent to Client(receiver) as reply | SENDMSG SERVER sender receiver data
    3.3 Packet sent to Client(sender) as reply   | SENDMSG-SUCCESS SERVER sender
    3.4 Packet sent to Client(sender) as reply   | SENDMSG-FAILURE SERVER sender details


4. Searching Database Protocols
--------------------------------

    Packet Type                                    | Packet format
    -----------------------------------------------|-----------------------------------------------
    4.1 Packet sent to Server                      | SEARCH-USER username SEVER name
    4.2 Packet sent to Client as reply (found)     | SEARCH-USER-RESULT SERVER username name
    4.3 Packet sent to Client as reply (not found) | SEARCH-USER-RESULT SERVER username

5. Adding friends Protocols
-----------------------------

    Packet Type                            | Packet format
    ---------------------------------------|-----------------------------------------------
    5.1 Packet sent to Server              | ADD-FRIEND username SERVER friendname
    5.2 Packet sent to Client as reply     | ADD-FRIEND-SUCCESS SERVER username
    5.3 Packet sent to Client as reply     | ADD-FRIEND-FAILURE SERVER username details

6. Notification Protocols
-----------------------------

    Packet Type                                                | Packet format
    -----------------------------------------------------------|-----------------------------------------------
    6.1.1 Packet sent to Client for a friend connecting       | NOTIF-LOGIN  SERVER username friendname
    6.1.2 Packet sent to Client for a friend disconnecting    | NOTIF-LOGOUT SERVER username friendname
    6.2.3 Packet sent to Server for currently online friends   | NOTIF-ONLINE username SERVER
    6.2.4 Packet sent to Client as reply                       | NOTIF-ONLINE SERVER username onlineFriends

