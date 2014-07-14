/* dispatch.c */
#include <stdio.h>
#include <stdlib.h>
#include "filter.h"
#include "init.h"

char *broadcast_to_local(char *cmd) 
{
	char *dest	= "127.0.0.1";
	int r = 0;
	r = broadcast_command_print(dest, cmd);
	if (r > 0) {
		printf("Can't reach the local daustd socket.\n");
		printf("Are you sure daustd is running?\n");
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
