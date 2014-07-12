/* dispatch.c */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "utils.h"

void init_dispatch(int argc, char *argv[], int optind)
{
	int i, w = 0, l = 0; 
	char *buf, *remote = NULL;
	for (i = optind; i < argc; i++) {
		if (w = 1) {
			remote = strdup(buf);	
			l = 1;
		}
		if (strcmp(buf, "remote")) {
			w = 1;
		}
		if (strcmp(buf, "all") == 0) {
			w = 2;	 
		}
		printf("Do something with: %s\n", argv[i]);
	} 
	//send
	if (remote) free(remote);	
}
