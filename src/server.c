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
#include "filter.h"
#include "utils.h"

int verify_node(char *local, char *foreign)
{
	// replace this with some kind of public/private 
	// key mechanism in the // future
	return strcmp(local, foreign) == 0 ? 1 : 0;
}

int process_command(char *cmd)
{

	char *rmt 		= NULL;
	if (strstr(cmd, "all")) {
		// do one of the 'all' commands
	} else if (strstr(cmd, "remote")) {
		// do one of the 'remote' commands
	} else if (strstr(cmd, "group")) {
		// do one of the 'group' commands
	} 
}

int check_command(struct nodeinfo *nfo)
{
	if (verify_node(nfo->identifier, config->identifier) > 0) {
		return 1;
	}


	char *cmd = NULL;
	cmd 		= nfo->command;
	int ac 		= 0;
	int *acp 	= &ac;
	char **av 	= explode(cmd, " ", acp); 

	printf("command is %s\n", cmd);

	// destroy explode array
	int i;
	for (i = 0; i < ac ; i++) {
		printf("seg %d is %s\n", i, av[i]);
		if (av[i]) free(av[i]);
	}
	free(av);
	if (cmd) free(cmd);

	return 0;
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
	printf("command is %s\n", nfo->command);
	if (nfo->command && strcmp(nfo->command, na) != 0) {
		//try to run command
		if (!check_command(nfo) && config->verbosity) {
			printf("Declined incoming command\n");
		}
	}
}

// receive data from other nodes
void init_server()
{
	init_nodelist();
	receive_packets(4040, incoming_callback);
	terminate_nodelist();
}
