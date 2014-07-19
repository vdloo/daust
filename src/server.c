/* server.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "server.h"
#include "server_dispatch.h"
#include "init.h"
#include "node_data.h"
#include "serialize.h"
#include "socket.h"

void print_received_nodelist(struct nli *nl)
{
	if (config->verbosity) {
		printf("received the following nodelist: \n");
		char *fbuf 	= NULL;
		fbuf 		= nodelist_list(nl);
		printf("%s\n", fbuf);
		free(fbuf);
	}
}

void print_local_nodelist()
{
	if (config->verbosity) {
		printf("local nodelist is now: \n");
		char *lbuf	= NULL;
		lbuf		= nodelist_list(head);
		printf("%s\n", lbuf);
		free(lbuf);
	}
}

// join incoming with local
void join_incoming(struct nli *nl)
{
	print_received_nodelist(nl);
	join_lists(head, nl);
	print_local_nodelist();
}

// create string to be broadcast back to client
char *create_response_buf(char *r)
{
	struct nli *nli;
	nli = create_self();
	set_node_element(&nli->info->command, r);
	if (r) free (r);

	char *res 	= NULL;
	res		= serialize(nli);
	destroy_nodelist(nli);
	return res;
}

void print_declined_incoming()
{
	if (config->verbosity) {
		printf("Declined incoming command\n");
	}
}

// the function to process incoming data
char *incoming_callback(char *buf)
{
	char *r = NULL;
	struct nli *nli;
	nli = deserialize(buf);
	join_incoming(nli);

	if (check_command(nli)) {
		r = server_dispatch(nli);
	} else {
		print_declined_incoming();
	}
	destroy_nodelist(nli);

	return create_response_buf(r);
}

// receive data from other nodes
void init_server()
{
	init_nodelist();
	if (config->verbosity) {
		printf("initialized nodelist: \n");
		char *lbuf	= NULL;
		lbuf		= nodelist_list(head);
		printf("%s\n", lbuf);
		free(lbuf);
	}
	receive_packets(4040, incoming_callback);
	terminate_nodelist();
}
