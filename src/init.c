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

	node = NULL;
       	node = create_nodelist();

	node->info = create_node();
	nfo = NULL;
	nfo = node->info;
	char *hn	= hostname();
	set_node_element(&nfo->hostname, 	hn);
	free(hn);
	set_node_element(&nfo->keynode, 	config->keynode);
	char *ih	= internalhost();
	set_node_element(&nfo->internalhost, 	ih);
	free(ih);
	set_node_element(&nfo->externalhost, 	config->publicface);
	set_node_element(&nfo->identifier, 	config->identifier);
	return node;
}

void init_nodelist()
{
	head 		= create_self();
}

// clean memory assigned to local nodelist
void terminate_nodelist()
{
	printf("cleaning up..\n");
	destroy_nodelist(head);
}

void print_received_nodelist(struct nli *nl)
{
	if (config->verbosity) {
		char *fbuf      = NULL;
		fbuf            = nodelist_list(nl);
		printf("%s\n", fbuf);
		free(fbuf);
	}
}

void print_local_nodelist()
{
	if (config->verbosity) {
		printf("local nodelist is now: \n");
		char *lbuf      = NULL;
		lbuf            = nodelist_list(head);
		printf("%s\n", lbuf);
		free(lbuf);
	}
}

void join_incoming(struct nli *nl)
{
	print_received_nodelist(nl);
	join_lists(head, nl);
	print_local_nodelist();
}

char *response_callback(char *buf)
{
	char *res	= NULL;
	if (buf) {
		struct nli *nli;
		struct nodeinfo *nfo;
		nli 		= deserialize(buf);
		if (nli) {
			if (head) join_incoming(nli);
			nfo 		= nli->info;
			res		= strdup(nfo->command);
			destroy_nodelist(nli);
		}
	}
	return res;
}

// broadcast nodelist to nodes in nodelist
void broadcast_nodelist(char *dest)
{
	char *buf;
	buf = serialize(head);
	if (buf) {
		send_packets(dest, 4040, buf, response_callback);
		free(buf);
	}
}

char *forward_command(char *dest, char *command, char *unique)
{
	set_node_element(&head->info->command, command);
	set_node_element(&head->info->unique, unique);

	char *buf;
	buf = serialize(head);

	char *r = NULL;
	r = send_packets(dest, 4040, buf, response_callback);
	if (buf) free(buf);
	return r;
}

char *broadcast_command(char *dest, char *command)
{
	struct nli *nli;
	nli = create_self();
	set_node_element(&nli->info->command, command);

	char *buf;
	buf = serialize(nli);
	destroy_nodelist(nli);

	char *r = NULL;
	if (buf) {
		r = send_packets(dest, 4040, buf, response_callback);
	}
	if (buf) free(buf);
	return r;
}
