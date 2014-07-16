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
	int r = 1;
	if (local && foreign) {
		r = !strcmp(local, foreign) == 0;
	}
	return r;
}

char *run_command(char *cmd)
{
	char *r	= NULL;
	char *first = strtok(cmd, " ");
	if (cmd) {
		if (strcmp(first, "stop") == 0) {
			if (config->verbosity) {
				printf("Received stop command. Goodbye.\n");
			}
			terminate_config();		
			exit(0);
		} else if (strcmp(first, "ping") == 0) {
			printf("pong\n");
			r = "pong";
		} else if (strcmp(first, "list") == 0) {
			r = log_nodelist(head);
		}
	}
	return r;
}

char *try_broadcast(char *dest, char *buf) {
	if (config->verbosity) {
		printf("Trying to broadcast %s to %s\n", buf, dest);
	}
	char *r 	= NULL;
	r  		= broadcast_command(dest, buf);
	if (r == NULL) {
		if (config->verbosity) {
			printf("Couldn't reach %s", dest);
		}
	}
	return r;
}

char *broadcast_to_remote(char *rmt, char *buf) {
	char *r = NULL;
	char *d = NULL;

	d 	= internalhost_by_hostname(rmt, head);
	if (d && strcmp(d, na) != 0) {
		r = try_broadcast(d, buf);
		if (r == NULL) {
			if (config->verbosity) {
				printf( "Will now try the external host\n");
			}
		} else {
			return r;
		}
	} else {
		if (config->verbosity) {
			printf(	"Found no matching internalhost, "
				"will now try to match for external\n");
		}
	}
	d 	= externalhost_by_hostname(rmt, head);
	if (d && strcmp(d, na) != 0) {
		r = try_broadcast(d, buf);
		if (r == NULL) {
			if (config->verbosity) {
				printf( "Will now try to contact node by address\n");
			}
		} else {
			return r;
		}
	} else {
		if (config->verbosity) {
			printf(	"Found no matching externalhost, "
				"will now try to contact node by address\n");
		}
	}
	r = try_broadcast(rmt, buf);
	if (r == NULL) {
		if (config->verbosity) {
			printf("Could not reach node by address\n");
		}
	} 
	return r;
}

char *check_then_run_command(struct nli *nl)
{
	struct nodeinfo *nfo;
	nfo = nl->info;
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

	if (who > 0) {
		join_lists(head, nl);
		if (config->verbosity) {
			printf("local nodelist is now: \n");
			char *lbuf	= NULL;
			lbuf		= log_nodelist(head);
			printf("%s\n", lbuf);
			free(lbuf);
		}
	}

	char *buf 	= NULL;
	buf		= sanitize_command(ac, av, 0);
	char *cbuf	= strdup(buf);
	if (buf == NULL) {
		return "no command";
	}

	// destroy array from explode
	int i;
	if (av) {
		for (i = 0; i < ac ; i++) {
			if (av[i]) free(av[i]);
		}
		free(av);
	}

	ac 		= 0;
	av		= explode(buf, " ", acp);
	if (buf) free(buf);
	char *cmd	= NULL;
	cmd 		= filter_command(ac, av, 0);

	char *r		= NULL;
	switch (who) {
		// run local
		case 0:
			// check if identifier matches
			if (verify_local(nfo->identifier, config->identifier)) {
				r = "your client ident doesn't match the server";
			} else {
				r = run_command(cmd);
			}
			break;
		// run on specified remote
		case 1:
			// check if this node is the specified node
			// otherwise, send it to that node
			if ( strcmp(rmt, head->info->hostname) == 0 ||
			     strcmp(rmt, head->info->internalhost) == 0 ||
		  	    (strcmp(na, head->info->externalhost) != 0 &&
			     strcmp(rmt, head->info->externalhost) == 0)) {
				r = run_command(cmd);
			} else {
				// contact rmt specified node
				r = broadcast_to_remote(rmt, cbuf);
				if (r == NULL) {
					r = "can not reach remote node";
				}
			}
			break;
		// run on all
		case 2: 
			// send the command to all nodes

			// run the command locally as well
			run_command(cmd);
			r = "Ran command aimed at all nodes";
			break;
	}
	if (cbuf) free(cbuf);

	for (i = 0; i < ac ; i++) {
		if (av[i]) free(av[i]);
	}
	if (av) free(av);

	if (cmd) free(cmd);

	return r;
}

// the function to process incoming data
char *incoming_callback(char *buf)
{
	char *r = NULL;
	struct nli *nl;
	nl = deserialize(buf);

	if (config->verbosity) {
		printf("received the following nodelist: \n");
		char *fbuf 	= NULL;
		fbuf 		= log_nodelist(nl);
		printf("%s\n", fbuf);
		if (fbuf) free(fbuf);
	}

	if (nl->info->command && strcmp(nl->info->command, na) != 0) {
		//try to run command
		r = check_then_run_command(nl);
		if (r == NULL && config->verbosity) {
			printf("Declined incoming command\n");
		}
	}

	struct nli *node;
	struct nodeinfo *rnfo;
	node = create_self();
	rnfo = node->info;
	set_node_element(&rnfo->command, strdup(r));
	return serialize(node);
}

// receive data from other nodes
void init_server()
{
	init_nodelist();
	if (config->verbosity) {
		printf("initialized nodelist: \n");
		char *lbuf	= NULL;
		lbuf		= log_nodelist(head);
		printf("%s\n", lbuf);
		free(lbuf);
	}
	receive_packets(4040, incoming_callback);
	terminate_nodelist();
}
