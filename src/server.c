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

// create string to be broadcast back to client
char *create_response_buf(char *r)
{
	set_node_element(&head->info->command, r);
	if (r) free (r);
	char *uq = gen_uuid();
	set_node_element(&head->info->unique, uq);
	free(uq);
	char *res 	= NULL;
	res		= serialize(head);
	return res;
}

void print_declined_incoming()
{
	print_verbose("Declined incoming command\n");
}

// nodelist of received and accepted commands
struct nli *rec_head;
struct nli *rec_tail;

// the function to process incoming data
char *incoming_callback(char *buf)
{
	char *r = NULL;
	if (buf) {
		struct nli *nli;
		nli = deserialize(buf);
		if (!nli) return NULL;
		join_incoming(nli);

		if (check_command(nli)) {
			r = server_dispatch(nli);
		} else {
			print_declined_incoming();
		}
		destroy_nodelist(nli);

		if (!r) {
			r = strdup("failed to forward command, possibly no other nodes available\n");
		}
		r = create_response_buf(r);
	}
	return r;
}

// receive data from other nodes
void init_server()
{
	init_nodelist();
	rec_head = add_node_to_list(NULL);
	rec_head->info = create_node();

	char *recl = strdup("Top of the received command list\n"); 
	set_node_element(&rec_head->info->command, recl);
	free(recl);

	rec_tail = rec_head;
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
