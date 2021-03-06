/* commands.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "utils.h"
#include "init.h"
#include "node_data.h"

void kill_daemon()
{
	print_verbose("Received stop command. Goodbye.\n");
	terminate_config();
	exit(0);
}

char *pong()
{
	print_verbose("pong\n");
	return strdup("pong\n");
}

char *trace()
{
	int m_siz = 0;
	int *mp = &m_siz;
	char *r;
	r = asdtobfp(r, mp, "__ ", head->info->hostname);
	r = asdtobfp(r, mp, "\n", NULL);
	return r;
}

char *list()
{
	char *r = NULL;
	r = nodelist_list(head);
	return r;
}
