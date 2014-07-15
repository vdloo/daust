/* init.h */
#ifndef INIT_H
#define INIT_H
void init_nodelist();
void broadcast_nodelist(char *dest);
void init_server();
struct nli *create_self();
void terminate_nodelist();
extern struct nli *head;
char *response_callback(char *buf);
char *broadcast_command_silent(char *dest, char *command);
char *broadcast_command_print(char *dest, char *command);
#endif
