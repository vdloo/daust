/* client_dispatch.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filter.h"
#include "init.h"
#include "messages.h"
#include "node_data.h"

void print_broadcast_response(char *r)
{
	if (r == NULL) {
		printf("Can't reach the local daustd socket.\n");
		printf("Are you sure daustd is running?\n");
	} else if (strcmp(r, na) == 0) {
		print_daust_usage();
	} else {
		printf("%s", r);
	}
}

void broadcast_to_local(char *cmd) 
{
	char *dest	= strdup("127.0.0.1");
	char *r 	= broadcast_command(dest, cmd);
	free(dest);

	print_broadcast_response(r);
	if (r) free(r);
}

void client_dispatch(int ac, char *av[], int o)
{
	head		= NULL;
	char *rmt 	= filter_specified_remote(ac, av, o);
	int who		= rmt ? 1 : filter_specified_all(ac, av, o);
	char *buf 	= filter_command(ac, av, o);
	char *cmd	= reconstruct_command(who, rmt, buf);
	if (rmt) free(rmt);
	if (buf) free(buf);
	broadcast_to_local(cmd);
	if (cmd) free(cmd);
}
