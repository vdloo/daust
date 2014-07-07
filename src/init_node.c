/* init.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "init_node.h"
#include "socket.h"

struct nodeinfo{
	char *hostname;
	char *internalhost;
	char *externalhost;
//      char *neighbours[2];
};

struct nodeinfo *createNode(char *hostname, char *internalhost, char *externalhost){
	struct nodeinfo *node = malloc(sizeof(struct nodeinfo));
	node->hostname = strdup(hostname);
	free(hostname);
	node->internalhost = strdup(internalhost);
	node->externalhost = strdup(externalhost);
	return node;
}

int init_node(){
	struct nodeinfo *nodeself = createNode(hostname(), internalhost(), "baas1");
	printf("nodeself->hostname is %s\n", nodeself->hostname);
	printf("nodeself->internalhost is %s\n", nodeself->internalhost);
	free(nodeself);
}
