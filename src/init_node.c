/* init_node.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "init_node.h"
#include "socket.h"

struct nodeinfo{
	char *hostname;
	char *internalhost;
	char *externalhost;
//      char *neighbours[2];
};

char *na = "N/A";
struct nodeinfo *createNode(char *hostname, char *internalhost, char *externalhost){
	struct nodeinfo *node = malloc(sizeof(struct nodeinfo));
	node->hostname = strdup(hostname); free (hostname);
	node->internalhost = internalhost ? strdup(internalhost) : na;
	node->externalhost = externalhost ? strdup(externalhost) : na;
	return node;
}

int init_node(){
	struct nodeinfo *nodeself = createNode(hostname(), internalhost(), config->publicfacing);
	printf("nodeself->hostname is %s\n", nodeself->hostname);
	printf("nodeself->internalhost is %s\n", nodeself->internalhost);
	printf("nodeself->publicfacing is %s\n", nodeself->externalhost);
	free(nodeself);
}
