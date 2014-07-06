#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
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

void print_version(){
	printf("daust 0.1\n");
}

void print_usage(){
	printf( "Usage: daust [options]\n"
		"	-V, --version		Print the version and exit.\n"
		"	-h, --help		Print this help and exit.\n\n"
		"Options: \n"
		"  -p, --publicfacing		Specify a public facing address for this node.\n"
		"  -v, --verbose			More verbose output\n"
	);
}

struct globalArgs_t{
	int verbosity;
	const char *publicfacing;
} globalArgs;

static const char *optString = "vp:Vh:";

static const struct option longOpts[] = {
	{ "version", no_argument, NULL, 'v' },
	{ "publicfacing", required_argument, NULL, 'p' },
	{ "verbose", no_argument, NULL, 'V' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};

int main(int argc, char *argv[]){
	int opt 			= 0;
	int longIndex			= 0;
	globalArgs.verbosity 		= 0;
	globalArgs.publicfacing 	= NULL;
	opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	while( opt != -1 ) {
		switch(opt) {
			case 'v':
				print_version();
				exit(0);
				break;

			case 'p':
				globalArgs.publicfacing = optarg;
				break;

			case 'V':
				globalArgs.verbosity++;
				break;
			case 'h':
				print_usage();
				exit(0);
				break;

			case '?':
			case ':':
			default:
				printf("nope");
				print_usage();
				exit(1);
				break;
		}
		opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	}

	struct nodeinfo *nodeself = createNode("kaas1", internalhost(), "baas1");
	free(nodeself);
	return 0;
}
