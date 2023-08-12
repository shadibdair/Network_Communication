# Network_Communication
Basic ideas of computing, networking, communications, security, and virtualization.


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