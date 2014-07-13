/* filter.c */
#include "string.h"
#include "utils.h"
#include "filter.h"

char *reconstruct_command(int w, char *rmt, char *command)
{
	char *buf = NULL;
	if (command) {
		int m_siz = 0;
		int *mp = &m_siz;
		if (w == 2) {
			buf = asdtobfp(buf, mp, "all", " ");
		} else if (rmt != NULL) {
			buf = asdtobfp(buf, mp, "remote", " ");
			buf = asdtobfp(buf, mp, rmt, " ");
		}
		buf = asdtobfp(buf, mp, command, '\0');
	}
	return buf;
}

// checks if a specific remote node is targeted
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

// checks if all remote nodes are targeted 
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

// filters command from destination
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
