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

// nodelist of received and accepted commands
struct nli *rec_head;
struct nli *rec_tail;

void print_received_command_list()
{
	if (config->verbosity) {
		printf("Appended incoming command to received list: \n");
		char *lbuf	= NULL;
		lbuf		= nodelist_list(rec_head->next);
		printf("%s\n", lbuf);
		free(lbuf);
	}
}

void append_to_received_command_list(struct nodeinfo *nfo)
{
	int rl = count_nodelist(rec_head);
	if (rl > 9) {
		remove_node_from_list(rec_head->next);
	}
	rec_tail = add_node_to_list(rec_tail);
	rec_tail->info = dup_nodeinfo(nfo);
	print_received_command_list();
}

// the function to process incoming data
char *incoming_callback(char *buf)
{
	char *r = NULL;
	struct nli *nli;
	nli = deserialize(buf);
	join_incoming(nli);

	if (check_command(nli)) {
		append_to_received_command_list(nli->info);
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
	rec_head = add_node_to_list(NULL);
	rec_head->info = create_node();
	char *recl = strdup("Top of the received command list\n"); 
	set_node_element(&rec_head->info->command, recl);
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
