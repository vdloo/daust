/* init.c */
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "init.h"
#include "node_data.h"
#include "serialize.h"
#include "socket.h"

struct nli *head;

// create local nodelist and initialize nodeinfo structure for self
void init_nodelist()
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
	head = node;

	//hn = strdup("neigh1");
	//kn = strdup("keynode1");
	//ih = strdup("192.168.1.2");
	//pf = strdup("n99.rickvandeloo.com");
	//id = strdup("uuid1");
	//np = add_node_to_list(np);
	//np->info 	= create_node(hn, kn, ih, pf, id);
	
	log_nodelist(head);
}

// broadcast nodelist to nodes in nodelist
void broadcast_nodelist()
{
	char *buf;
	buf = serialize(head);
	send_packets("test", 4040, buf);
	free(buf);
}

// the function to process incoming data
void incoming_callback(char *buf)
{
	struct nli *nl = deserialize(buf);
	int nodesleft = count_nodelist(nl);
	if (config->verbosity) {
		printf("received the following nodelist:\n");
		log_nodelist(nl);
		printf("local nodelist is now:\n");
		log_nodelist(head);
	}
}

// receive data from other nodes
void init_server()
{
	receive_packets(4040, incoming_callback);
}

// clean memory assigned to local nodelist
void terminate_nodelist()
{
	//printf("cleaning up..\n");
	destroy_nodelist(head);
}
