/* init_node.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "init_node.h"
#include "socket.h"

struct nodeinfo {
	char *hostname;
	char *internalhost;
	char *externalhost;
//      char *neighbours[2];
};

char *na = "N/A";
struct nodeinfo *createNode(char *hn, char *ih, char *eh)
{
	struct nodeinfo *node = malloc(sizeof(struct nodeinfo));
	node->hostname = strdup(hn); free (hn);
	node->internalhost = ih ? strdup(ih) : na;
	node->externalhost = eh ? strdup(eh) : na;
	return node;
}

int init_node()
{
	struct nodeinfo *nodeself;
	nodeself = createNode(hostname(), internalhost(), config->publicfacing);
	printf("nodeself->hostname is %s\n", nodeself->hostname);
	printf("nodeself->internalhost is %s\n", nodeself->internalhost);
	printf("nodeself->publicfacing is %s\n", nodeself->externalhost);
	free(nodeself);
}
