/* init.h */
#ifndef INIT_H
#define INIT_H
void init_nodelist();
void broadcast_nodelist();
void init_server();
struct nli *create_self();
void terminate_nodelist();
extern struct nli *head;
char *response_callback(char *buf);
int broadcast_command(char *command);
#endif
