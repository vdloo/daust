/* init.h */
#ifndef INIT_H
#define INIT_H
void init_nodelist();
void broadcast_nodelist();
void init_server();
void terminate_nodelist();
extern struct nli *head;
#endif
