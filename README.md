# Network_Communication
Basic ideas of computing, networking, communications, security, and virtualization.

-------------------------------------

# ping_icmp_protocol.c Code 🔥

The code represents an implementation of the "ping" command using the ICMP (Internet Control Message Protocol). This protocol is used to send control and error messages between hosts and routers on the Internet.

1. **Socket Creation:** The code creates a RAW socket with the ICMP protocol, thus allowing direct control over the packet being sent.
2. **Packet Preparation:** The code prepares an ICMP packet of type ECHO REQUEST. It defines the packet type, code, checksum, unique identifier, and sequence number.
3. **Checksum Calculation:** The checksum function calculates the checksum for the packet, enabling the recipient of the packet to verify that there were no errors during its transmission.
4. **Packet Sending:** The code sends the request packet to the IP address given as a command-line parameter.
5. **Response Reception:** The code waits for a response from the receiving server and prints a message with the IP address of the server that returned the response.

In summary, the code performs a reachability check to a remote server by sending a ping request and receiving a response. It simulates the operation of the well-known ping command found in operating systems and allows for a quick check of accessibility to a specific host on the network.

### How To Run The Code :

- Compile the code : 

```
gcc -o ping_example ping_icmp_protocol.c
```

- Run the code :

```
sudo ./ping_example 8.8.8.8
```


----------------------------------

# traceroute.c Code 🔥
### Description:

The provided code is an implementation of the traceroute utility in C. Traceroute is a network diagnostic tool used to trace the path that an IP packet follows from the source to the destination. It identifies all the routers (hops) on the path, giving a view of the network topology and how data travels through the network.

### How the Code Works:

1. **Initialization and Argument Checking**: The code starts by ensuring that the proper command-line arguments are provided, expecting an IP address as the target of the traceroute.

2. **Socket Creation**: A raw socket is created to handle the ICMP protocol, which is used to send and receive control messages related to network operations.

3. **Target Initialization**: The target IP address is extracted from the command-line arguments and stored in a `sockaddr_in` structure.

4. **Probing Loop**: The core logic of the traceroute is performed in a loop, iterating through the defined maximum number of hops (MAX_HOPS, set to 30 in the code). Within each iteration:
   
   a. **Send Probe**: An ICMP Echo Request is sent to the target with a specific TTL (Time to Live) value. The TTL is set to the current hop number, causing the packet to expire and be dropped by the router at that hop. The router then responds with an ICMP Time Exceeded message.

   b. **Checksum Calculation**: The code includes a function to calculate the ICMP checksum to ensure packet integrity.

   c. **Waiting for Reply**: The program waits for a reply using the `select` system call. If there's no reply within a given time frame, it indicates a timeout for that hop.

   d. **Processing Reply**: If a reply is received, the code examines the ICMP type to determine if it's a Time Exceeded message (from an intermediate router) or an Echo Reply (from the destination itself). The IP address of the replying router is printed.

   e. **Destination Check**: If the reply is an Echo Reply, the loop breaks, indicating that the destination has been reached.

5. **Cleanup**: Finally, the raw socket is closed, and the program exits.

### Summary:

The code provides a concise yet powerful tool to trace the path from the host system to a given IP address. It's an educational example of how to build a network diagnostic tool that reveals the underlying structure of the Internet, one hop at a time. It demonstrates the handling of raw sockets, ICMP messages, and network time-outs, and it provides valuable insights into network behavior and routing.

### How To Run The Code :

- Compile the code : 

```
gcc -o traceroute traceroute.c
```

- Run the code :

```
sudo ./traceroute 8.8.8.8
```
