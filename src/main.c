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
	char *localhost;
	char *remotehost;
//	char *neighbours[2];
};

struct nodeinfo *createNode(char *hostname, char *localhost, char *remotehost){
	struct nodeinfo *node = malloc(sizeof(struct nodeinfo));
	node->hostname = strdup(hostname);
	node->localhost = strdup(localhost);
	node->remotehost = strdup(remotehost);
	return node;
}

int main(){
	printf("hello world\n");
	struct nodeinfo *nodeself = createNode("kaas", "is", "baas");
	printf("nodeself->hostname is %s", nodeself->hostname);
	return 0;
}
