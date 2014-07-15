#ifndef SOCKET_H
#define SOCKET_H
int receive_packets(int port, char *(*cb)(char *buf));
char *send_packets(char *host, int port, char *buf, char *(*cb)(char *param));
char *hostname();
char *internalhost();
#endif
