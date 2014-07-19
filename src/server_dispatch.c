/* server_dispatch.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "init.h"
#include "node_data.h"
#include "filter.h"
#include "utils.h"
#include "commands.h"

int verify_local(char *local, char *foreign)
{
	int r = 1;
	if (local && foreign) {
		r = !strcmp(local, foreign) == 0;
	}
	return r;
}

int check_if_stop(char *first)
{
	return strcmp(first, "stop") == 0;
}

int check_if_ping(char *first)
{
	return strcmp(first, "ping") == 0;
}

int check_if_list(char *first)
{
	return strcmp(first, "list") == 0;
}

char *run_command(char *cmd)
{
	char *r	= NULL;
	char *first = strtok(cmd, " ");
	if (cmd) {
		if (check_if_stop(first)) kill_daemon();
		if (check_if_ping(first)) r = pong();
		if (check_if_list(first)) r = nodelist_list(head);
	}
	return r;
}

void print_try_broadcast(char *dest, char *buf)
{
	if (config->verbosity) {
		printf("Trying to broadcast %s to %s\n", buf, dest);
	}
}

void print_failed_broadcast(char *dest)
{
	if (config->verbosity) {
		printf("Couldn't reach %s", dest);
	}
}

// try to broadcast buf to destination dest
char *try_broadcast(char *dest, char *buf) {
	print_try_broadcast(dest, buf);

	char *r 	= broadcast_command(dest, buf);

	if (r == NULL) print_failed_broadcast(dest);
	return r;
}

void print_try_external()
{
	if (config->verbosity) {
		printf( "Will now try the external host\n");
	}
}

void print_failed_internal()
{
	if (config->verbosity) {
		printf(	"Found no matching internalhost, "
			"will now try to match for external\n");
	}
}

void print_try_address()
{
	if (config->verbosity) {
		printf( "Will now try to contact node by address\n");
	}
}

void print_failed_external()
{
	if (config->verbosity) {
		printf(	"Found no matching externalhost, "
			"will now try to contact node by address\n");
	}
}

void print_failed_address()
{
	if (config->verbosity) {
		printf("Could not reach node by address\n");
	}
}

// if internalhost is found matching a hostname,
// try connecting to this address
char *try_internal(char *d, char *buf)
{
	char *r = NULL;
	if (d && strcmp(d, na) != 0) {
		r = try_broadcast(d, buf);
		if (r == NULL) {
			print_try_external();
		} else {
			return r;
		}
	} else {
		print_failed_internal();
	}
	return r;
}

// if externalhost is found matching a hostname,
// try connecting to this address
char *try_external(char *d, char *buf)
{
	char *r = NULL;
	if (d && strcmp(d, na) != 0) {
		r = try_broadcast(d, buf);
		if (r == NULL) {
			print_try_address();
		} else {
			return r;
		}
	} else {
		print_failed_external();
	}
	return r;
}

// try connecting using the buf as the address
char *try_address(char *d, char *buf)
{
	char *r = NULL;
	r = try_broadcast(d, buf);
	if (r == NULL) {
		print_failed_address();
	} 
	return r;
}

// try to broadcast the message to a node by first
// checking if there is a known and responsive internalhost 
// for this hostname, then the same for a known externalhost
// and then if those fail try to connect using the hostname
// as the address. So first time connecting with 
// 	$ daust remote 192.168.50.3 
// will result in directly connecting to that node by ip address
char *broadcast_to_remote(char *rmt, char *buf) {
	char *r = NULL;
	char *d = NULL;
	d = internalhost_by_hostname(rmt, head);
	r = try_internal(d, buf);
	if (r == NULL) {
		d = externalhost_by_hostname(rmt, head);
		r = try_external(d, buf);
	}
	if (r == NULL) {
		r = try_address(rmt, buf);	
	}
	return r;
}

// check if command is not the placeholder
int chck_na_cmd(struct nli* nl)
{
	return strcmp(nl->info->command, na) == 0;
}

// check if command is not NULL
int chck_no_cmd(struct nli *nl)
{
	return nl->info->command == NULL;
}

// check if the node matches the authorization criteria
// to contact this node
int auth_nli(struct nli *nl)
{
	struct nodeinfo *nfo;
	nfo = nl->info;
	// do something with public and private keys
	// here in the future
//	return auth_node(nfo->identifier, config->identifier) > 0;
	return 0;
}

// check if identifier matches, then run command
char *run_local(struct nli *nli, char *cmd)
{
	char *r;
	if (verify_local(nli->info->identifier, config->identifier)) {
		r = strdup("your client ident doesn't match the server");
	} else {
		r = run_command(cmd);
	}
	return r;
}

// check if this node is the specified node
// otherwise, send it to that node
char *run_remote(char *rmt, char *buf, char *cmd)
{
	char *r;
	if ( strcmp(rmt, head->info->hostname) == 0 ||
	     strcmp(rmt, head->info->internalhost) == 0 ||
	    (strcmp(na, head->info->externalhost) != 0 &&
	     strcmp(rmt, head->info->externalhost) == 0)) {
		r = run_command(cmd);
	} else {
		// contact rmt specified node
		r = broadcast_to_remote(rmt, buf);
		if (r == NULL) {
			r = strdup("can not reach remote node");
		}
	}
	return r;
}

char *run_all(char *cmd)
{
	char *r;
	// send the command to all nodes

	// run the command locally as well
	run_command(cmd);
	r = strdup("Ran command aimed at all nodes");
	return r;
}

// run the command or send it to the right nodes
char *route_command(struct nli *nli, int who, char *rmt, char *cmd, char *buf)
{
	char *r		= NULL;
	switch (who) {
		// run local
		case 0:
			r = run_local(nli, cmd);
			break;
		// run on specified remote
		case 1:
			r = run_remote(rmt, buf, cmd);
			break;
		// run on all
		case 2: 
			r = run_all(cmd);
			break;
	}
	return r;
}

// test is the incomming command looks like a command and then
// calls route_command to send it to the right node
int check_command(struct nli *nli)
{
	// return NULL if no command is present in the node list item
	if (chck_no_cmd(nli)) 	return 0;
	if (chck_na_cmd(nli)) 	return 0;

	// return NULL if node list item can't be authenticated
	if (auth_nli(nli) > 0) 	return 0; 

	return 1;
}

char *server_dispatch(struct nli *nli)
{

	// put incoming command into array
	int ac 		= 0;
	int *acp 	= &ac;
	char **av 	= explode(nli->info->command, " ", acp); 

	char *rmt 	= NULL;
	rmt 		= filter_specified_remote(ac, av, 0);

	// find out what group the command needs to be sent to
	int who		= filter_who(ac, av, 0, rmt);


	// put the incoming command in the expected order
	char *buf 	= NULL;
	buf		= sanitize_command(ac, av, 0);
	if (buf == NULL) {
		return "no command";
	}

	// free all memory of the first explode array
	destroy_array(av, ac);

	// put sanitized command into array
	ac 		= 0;
	av		= explode(buf, " ", acp);

	// filter the destination out of the command
	char *cmd	= NULL;
	cmd 		= filter_command(ac, av, 0);

	// free all memory of the second explode array
	destroy_array(av, ac);

	// run the command or 
	// send it to the right nodes
	char *r;
	r = route_command(nli, who, rmt, cmd, buf);

	if (rmt) free(rmt);
	if (buf) free(buf);
	if (cmd) free(cmd);

	return r;
}
