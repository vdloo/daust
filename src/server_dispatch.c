/* server_dispatch.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// for sleep
#include <unistd.h>

#include "server.h"
#include "config.h"
#include "init.h"
#include "node_data.h"
#include "filter.h"
#include "utils.h"
#include "socket.h"
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

int check_if_trace(char *first)
{
	return strcmp(first, "trace") == 0;
}

// debug function, remove later
// use this to test blocking socket threads
int check_if_block(char *first)
{
	return strcmp(first, "block") == 0;
}

char *block()
{
	sleep(5);
	return strdup("slept for 5 seconds");
}

char *run_command(char *cmd)
{
	char *r	= NULL;
	char *first = strtok(cmd, " ");
	if (cmd) {
		if (check_if_stop(first)) kill_daemon();
		if (check_if_ping(first)) r = pong();
		if (check_if_list(first)) r = nodelist_list(head);
		if (check_if_block(first)) r = block();
		if (check_if_trace(first)) r = trace();
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
		printf("Couldn't reach %s\n", dest);
	}
}

// try to broadcast buf to destination dest
char *try_broadcast(char *dest, char *buf, char *unique) {
	print_try_broadcast(dest, buf);

	char *r 	= forward_command(dest, buf, unique);

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
char *try_internal(char *d, char *buf, char *uq)
{
	char *r = NULL;
	if (d && strcmp(d, na) != 0) {
		r = try_broadcast(d, buf, uq);
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
char *try_external(char *d, char *buf, char *uq)
{
	char *r = NULL;
	if (d && strcmp(d, na) != 0) {
		r = try_broadcast(d, buf, uq);
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
char *try_address(char *d, char *buf, char *uq)
{
	char *r = NULL;
	r = try_broadcast(d, buf, uq);
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
char *broadcast_to_remote(char *rmt, char *buf, char *uq) {
	char *r = NULL;
	char *d = NULL;
	d = internalhost_by_hostname(rmt, head);
	r = try_internal(d, buf, uq);
	if (r == NULL) {
		d = externalhost_by_hostname(rmt, head);
		r = try_external(d, buf, uq);
	}
	if (r == NULL) {
		r = try_address(rmt, buf, uq);	
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
char *run_remote(char *rmt, char *buf, char *cmd, char *uq)
{
	char *r;
	if ( strcmp(rmt, head->info->hostname) == 0 ||
	     strcmp(rmt, head->info->internalhost) == 0 ||
	    (strcmp(na, head->info->externalhost) != 0 &&
	     strcmp(rmt, head->info->externalhost) == 0)) {
		r = run_command(cmd);
	} else {
		// contact rmt specified node
		r = broadcast_to_remote(rmt, buf, uq);
		if (r == NULL) {
			r = strdup("can not reach remote node");
		}
	}
	return r;
}

// send the command to all nodes, 
// skip first to not send to self
char *broadcast_to_all(struct nli *nli, char *buf, char *uq)
{
	char *rbuf;
	char *hn = NULL;
	int m_siz = 0;
	int *mp = &m_siz;

	char *r = NULL;

	do
	while (nli = nli->next)
	{
		hn = nli->info->hostname;
		rbuf = NULL;
		rbuf = broadcast_to_remote(hn, buf, uq);
		if (rbuf) {
			if (strcmp(rbuf, "Already have it") != 0) {
				r = asdtobfp(r, mp, rbuf, "\n\n");
				r = astobfp(r, mp, NULL);
			}
			free(rbuf);
		}
	}
	return r;
}

// broadcast command to all available nodes, then
// run the command locally. All output appended to buffer r
char *run_all(struct nli *nli, char *cmd, char *buf, char *uq)
{
	char *r = NULL;
	int m_siz = 0;
	int *mp = &m_siz;
	char *rbuf;

	// send the command to all other known nodes
	rbuf = broadcast_to_all(head, buf, uq);
	if (rbuf) {
		r = astobfp(r, mp, rbuf);
		free(rbuf);
	}

	// run the command locally
	char *m = strdup(" responded:\n");	
	r = asdtobfp(r, mp, head->info->hostname, m);
	free(m);
	rbuf = run_command(cmd);
	r = astobfp(r, mp, rbuf);
	free(rbuf);
	r = astobfp(r, mp, NULL);
	return r;
}

// run the command or send it to the right nodes
char *route_command(struct nli *nli, int who, char *rmt, char *cmd, char *buf, char *uq)
{
	char *r		= NULL;
	switch (who) {
		// run local
		case 0:
			r = run_local(nli, cmd);
			break;
		// run on specified remote
		case 1:
			r = run_remote(rmt, buf, cmd, uq);
			break;
		// run on all
		case 2: 
			r = run_all(nli, cmd, buf, uq);
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

// nodelist of received and accepted commands
void print_received_command_list()
{
	if (config->verbosity) {
		printf("Appended incoming command to received list: \n");
		char *lbuf      = NULL;
		lbuf            = nodelist_list(rec_head->next);
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
		return strdup("no command");
	}
	set_node_element(&nli->info->command, buf);

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

	char *uq = strdup(nli->info->unique);

	// if nli->info isn't in rec_head nodelist
	// run the command or 
	// send it to the right nodes
	char *r;
	if (find_node(nli, rec_head) == NULL) {
		append_to_received_command_list(nli->info);
		r = route_command(nli, who, rmt, cmd, buf, uq);
	} else {
		r = strdup("Already have it"); 	
	}

	if (uq) free(uq);
	if (rmt) free(rmt);
	if (buf) free(buf);
	if (cmd) free(cmd);

	return r;
}
