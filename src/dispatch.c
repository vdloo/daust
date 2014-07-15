/* dispatch.c */
#include <stdio.h>
#include <stdlib.h>
#include "filter.h"
#include "init.h"
#include "messages.h"
#include "node_data.h"

char *broadcast_to_local(char *cmd) 
{
	char *dest	= "127.0.0.1";
	char *r 	= NULL;;
	r = broadcast_command(dest, cmd);
	if (r == NULL) {
		printf("Can't reach the local daustd socket.\n");
		printf("Are you sure daustd is running?\n");
	} else if (strcmp(r, na) == 0) {
		print_daust_usage();
	} else {
		printf("%s\n", r);
	}
}

void init_dispatch(int ac, char *av[], int o)
{
	char *rmt 	= NULL;
	rmt		= filter_specified_remote(ac, av, o);
	int who		= rmt ? 1 : filter_specified_all(ac, av, o);
	char *buf 	= NULL;
	buf 		= filter_command(ac, av, o);
	char *cmd	= NULL;
	cmd		= reconstruct_command(who, rmt, buf);
	if (rmt) free(rmt);
	if (buf) free(buf);
	broadcast_to_local(cmd);
	if (cmd) free(cmd);
}
