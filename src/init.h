/* init.h */
#ifndef INIT_H
#define INIT_H
void init_nodelist();
void broadcast_nodelist(char *dest);
void init_server();
struct nli *create_self();
void terminate_nodelist();
extern struct nli *head;
void join_incoming(struct nli *nl);
char *response_callback(char *buf);
char *broadcast_command(char *dest, char *command);
#endif
