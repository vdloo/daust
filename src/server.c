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

int auth_node(char *local, char *foreign)
{
	// replace this with some kind of public/private 
	// key mechanism in the // future
	return 0;
}

int verify_local(char *local, char *foreign)
{
	return !strcmp(local, foreign) == 0;
}

char *run_command(char *cmd)
{
	char *r	= NULL;
	if (cmd) {
		if (strcmp(cmd, "stop") == 0) {
			printf("Received stop command. Goodbye.\n");
			terminate_config();		
			exit(0);
		}	
	}
}

char *check_then_run_command(struct nodeinfo *nfo)
{
	if (auth_node(nfo->identifier, config->identifier) > 0) {
		return NULL;
	}

	// put incoming options into array
	int ac 		= 0;
	int *acp 	= &ac;
	char **av 	= explode(nfo->command, " ", acp); 

	char *rmt 	= NULL;
	rmt 		= filter_specified_remote(ac, av, 0);
	int who		= filter_who(ac, av, 0, rmt);

	char *cmd 	= NULL;
	cmd		= sanitize_command(ac, av, 0);

	char *r		= NULL;
	switch (who) {
		// run local
		case 0:
			// check if identifier matches
			if (verify_local(nfo->identifier, config->identifier)) {
				r = NULL;
			} else {
				run_command(cmd);
				r = "Ran local command";
			}
			break;
		// run on specified remote
		case 1:
			// check if this node is the specified node
			r = "Ran command aimed at specific node";
			break;
		// run on all
		case 2: 
			r = "Ran command aimed at all nodes";
			break;
	}

	// destroy array from explode
	int i;
	for (i = 0; i < ac ; i++) {
		if (av[i]) free(av[i]);
	}
	free(av);
	if (cmd) free(cmd);

	return r;
}

// the function to process incoming data
char *incoming_callback(char *buf)
{
	char *r = NULL;
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
	if (nfo->command && strcmp(nfo->command, na) != 0) {
		//try to run command
		r = check_then_run_command(nfo);
		if (r == NULL && config->verbosity) {
			printf("Declined incoming command\n");
		}
	}
	return r;
}

// receive data from other nodes
void init_server()
{
	init_nodelist();
	receive_packets(4040, incoming_callback);
	terminate_nodelist();
}
