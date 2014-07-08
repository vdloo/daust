/* init_node.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "init_node.h"
#include "socket.h"

struct nodeinfo {
	char *hostname; 	// hostname of the machine
	char *keynode;		// node that introduced this node to the cluster
	char *internalhost;	// local ip of node
	char *externalhost;	// public facing ip of the node
	char *neighbour[2];	// nodes that this node talks to
};

char *na = "N/A";
struct nodeinfo *create_node(char *hn, char *kn, char *ih, char *eh)
{
	struct nodeinfo *node 	= malloc(sizeof(struct nodeinfo));
	node->hostname 		= strdup(hn); free (hn);
	node->internalhost 	= ih ? strdup(ih) : na;
	node->keynode		= kn ? strdup(kn) : na;
	node->externalhost 	= eh ? strdup(eh) : na;
	node->neighbour[0] 	= na;
	node->neighbour[1] 	= na;
	return node;
}

int init_node()
{
	struct nodeinfo *nodeself;
	char *hn = hostname();
	char *kn = config->keynode;
	char *ih = internalhost();
	char *pf = config->publicface;
	nodeself = create_node(hn, kn, ih, pf);
	printf("nodeself->hostname is %s\n", nodeself->hostname);
	printf("nodeself->keynode is %s\n", nodeself->keynode);
	printf("nodeself->internalhost is %s\n", nodeself->internalhost);
	printf("nodeself->publicface is %s\n", nodeself->externalhost);
	printf("nodeself->neighbour 1 is %s\n", nodeself->neighbour[0]);
	printf("nodeself->neighbour 2 is %s\n", nodeself->neighbour[1]);

	send("test", 4040, "this is a test string");

	free(nodeself);
	printf("now exiting init_node()\n");
}
