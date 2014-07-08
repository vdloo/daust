#ifndef SOCKET_H
#define SOCKET_H
int receive_packets(int port);
int send_packets(char *host, int port, char *buf);
char *hostname();
char *internalhost();
#endif
