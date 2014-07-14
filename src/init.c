/* init.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "init.h"
#include "node_data.h"
#include "serialize.h"
#include "socket.h"
#include "messages.h"

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

char *response_callback_silent(char *buf)
{
	char *r = NULL;
	struct nli *nl;
	struct nodeinfo *nfo;
	nl = deserialize(buf);
	nfo = nl->info;
	return nfo->command;
}

char *response_callback_print(char *buf)
{
	char *res 	= NULL;
	res 		= response_callback_silent(buf);
	if (strcmp(res, na) == 0) {
		print_daust_usage();
	} else {
		printf("%s\n", res);
	}
	free(res);
}

// broadcast nodelist to nodes in nodelist
void broadcast_nodelist(char *dest)
{
	char *buf;
	buf = serialize(head);
	send_packets(dest, 4040, buf, response_callback_silent);
	free(buf);
}

int broadcast_command_silent(char *dest, char *command)
{
	struct nli *node;
	struct nodeinfo *nfo;
	node = create_self();
	nfo = node->info;
	set_node_element(&nfo->command,	command);

	char *buf;
	buf = serialize(node);

	int r = 0;
	r = send_packets(dest, 4040, buf, response_callback_silent);
	free(buf);
	return r;
}

int broadcast_command_print(char *dest, char *command)
{
	struct nli *node;
	struct nodeinfo *nfo;
	node = create_self();
	nfo = node->info;
	set_node_element(&nfo->command,	command);

	char *buf;
	buf = serialize(node);

	int r = 0;
	r = send_packets(dest, 4040, buf, response_callback_print);
	free(buf);
	return r;
}
