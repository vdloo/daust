#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket.h"

void error(const char *msg){
	perror(msg);
}

struct nodeinfo{
	char *hostname;
	char *internalhost;
	char *externalhost;
//	char *neighbours[2];
};

struct nodeinfo *createNode(char *hostname, char *internalhost, char *externalhost){
	struct nodeinfo *node = malloc(sizeof(struct nodeinfo));
	node->hostname = strdup(hostname);
	node->internalhost = strdup(internalhost);
	node->externalhost = strdup(externalhost);
	return node;
}

void print_usage() {
	printf(	"Usage: daust [options]\n"
		"	-v, --version		Print the version and exit.\n"
		"	-h, --help		Print this help and exit.\n\n"
		"Options: \n"
		"  -r, --remote			Specify a public facing address for this node."
	);
}

int main(){
















	struct nodeinfo *nodeself = createNode("kaas1", internalhost(), "baas1");
	free(nodeself);
	return 0;
}
