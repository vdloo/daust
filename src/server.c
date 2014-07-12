/* server.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "server.h"
#include "init.h"
#include "node_data.h"
#include "serialize.h"
#include "socket.h"

int verify_node(char *local, char *foreign)
{
	// replace this with some kind of public/private 
	// key mechanism in the // future
	return strstr(local, foreign) ? 1 : 0;
}


int check_command(struct nodeinfo *nfo)
{
	printf("checking the incoming command: %s\n", nfo->command);

	int i = 0;
	if (strstr(nfo->command, "all")) {
		// do one of the 'all' commands
	} else if (strstr(nfo->command, "remote")) {
		// do one of the 'remote' commands
	} else if (strstr(nfo->command, "group")) {
		// do one of the 'group' commands
	} else {
		// do one of the 'self' commands
		if (verify_node(nfo->identifier, config->identifier)) {
//			i = run_command(nfo->command);
		}
	}
	return i;
}

// the function to process incoming data
char *incoming_callback(char *buf)
{
	struct nli *nl;
	struct nodeinfo *nfo;
       	nl = deserialize(buf);

	if (config->verbosity) {
		printf("received the following nodelist:\n");
		log_nodelist(nl);
		printf("local nodelist is now:\n");
		log_nodelist(head);
	}

	nfo = nl->info;
	if (nfo->command && !strstr(nfo->command, na)) {
		//try to run command
		if (!check_command(nfo) && config->verbosity) {
			printf("Declined incoming command\n");
		}
	}
}

// receive data from other nodes
void init_server()
{
	receive_packets(4040, incoming_callback);
}
