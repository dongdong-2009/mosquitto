#ifndef __HTTP_H__
#define __HTTP_H__

//#define IPSTR "120.24.38.104"
//#define PORT 80
#define BUFSIZE 1024

// Create TCP connection and connect
int net_connet(char *IP, int port);

// Disconnect TCP connection
int net_disconn(int socked);

// Send data
int net_send(int socked, char *buf_send, int len);

// Recv data
int net_recv(int socked, char *buf_recv, int len);

#endif  //end of __HTTP_H__

