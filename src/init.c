/* init.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "init.h"
#include "node_data.h"
#include "serialize.h"
#include "socket.h"

struct nli *head;

// create local nodelist and initialize nodeinfo structure for self
struct nli *create_self()
{
	struct nli *node;
	struct nodeinfo *nfo;

       	node = create_nodelist();

	node->info = create_node();
	nfo = node->info;
	set_node_element(&nfo->hostname, 	hostname());
	set_node_element(&nfo->keynode, 	config->keynode);
	set_node_element(&nfo->internalhost, 	internalhost());
	set_node_element(&nfo->externalhost, 	config->publicface);
	set_node_element(&nfo->identifier, 	config->identifier);
	return node;
}
void init_nodelist()
{
	head = create_self();
	log_nodelist(head);
}

// clean memory assigned to local nodelist
void terminate_nodelist()
{
	//printf("cleaning up..\n");
	destroy_nodelist(head);
}

// broadcast nodelist to nodes in nodelist
void broadcast_nodelist()
{
	char *buf;
	buf = serialize(head);
	send_packets("test", 4040, serialize(head));
	free(buf);
}

void broadcast_command(char *command)
{
	char *buf;
	struct nli *node;
	struct nodeinfo *nfo;
	node = create_self();
	nfo = node->info;

	set_node_element(&nfo->command,	command);

	buf = serialize(node);
	send_packets("test", 4040, serialize(node));
	free(buf);
}
