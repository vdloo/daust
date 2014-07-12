/* dispatch.c */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "utils.h"

char *mend_command(int w, char *rmt, char *command)
{
	char *buf = NULL;
	if (command) {
		int m_siz = 0;
		int *mp = &m_siz;
		printf("remote is %s\n", rmt);
		if (w == 2) {
			buf = asdtobfp(buf, mp, "all", " ");
		} else if (rmt != NULL) {
			printf("now appending remote");
			buf = asdtobfp(buf, mp, "remote", " ");
			buf = asdtobfp(buf, mp, rmt, " ");
		}
		buf = asdtobfp(buf, mp, command, '\0');
	}
	return buf;
}

// filters destination from command
char *filter_specified_remote(int ac, char *av[], int o)
{
	char *rmt = NULL;
	int i;
	for (i = o; i < ac; i++) {
		if (strcmp(av[i], "remote") == 0) {
			if ((i + 1) < ac) {
				if (!rmt) rmt = strdup(av[i+1]);
				i++;
			}
		}
	} 
	return rmt;
}

int filter_specified_all(int ac, char *av[], int o)
{
	int i, a = 0;
	for (i = o; i < ac; i++) {
		if (strcmp(av[i], "all") == 0) {
			a = 2;
		}
	} 
	return a;
}

char *filter_command(int ac, char *av[], int o)
{
	int a 		= 0;
	int j 		= 0;
	char *sg, *buf 	= NULL;
	int m_siz 	= 0;
	int *mp = &m_siz;
	int i;
	for (i = o; i < ac; i++) {
		sg = av[i];	
		if (	!(strcmp(sg, "all") == 0) &&
			!(strcmp(sg, "remote") == 0) &&
			j == 0
		) {
			buf = asdtobfp(buf, mp, sg, " ");
		} else if (strcmp(sg, "remote") == 0) {
			// skip next arg if current is "remote"
			j = 1;
		} else {
			j = 0;
		}
	} 
	buf = astobfp(buf, mp, NULL);
	return buf;
}

char *format_command(int ac, char *av[], int o)
{
	char *rmt 	= NULL;
	rmt		= filter_specified_remote(ac, av, o);
	// 0 is local, 1 is specified remote, 2 is all
	int who		= rmt ? 1 : filter_specified_all(ac, av, o);
	char *buf 	= NULL;
	buf 		= filter_command(ac, av, o);
	char *cmd	= NULL;
	cmd		= mend_command(who, rmt, buf);
	if (rmt) free(rmt);
	if (buf) free(buf);
	return cmd;
}

void init_dispatch(int argc, char *argv[], int optind)
{
	char *cmd 	= NULL;
	cmd 		= format_command(argc, argv, optind);

	printf("now broadcasting command: %s\n", cmd);
	broadcast_command(cmd);
	if (cmd) free(cmd);
}
