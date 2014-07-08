/* main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "config.h"
#include "messages.h"
#include "init_node.h"

void error(const char *msg)
{
	perror(msg);
}

struct conf *config;
static const char *optString = "vp:Vh:";
static const struct option longOpts[] = {
	{ "version", no_argument, NULL, 'v' },
	{ "publicface", required_argument, NULL, 'p' },
	{ "verbose", no_argument, NULL, 'V' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};


int main(int argc, char *argv[])
{
	int opt 			= 0;
	int longIndex			= 0;
	config				= malloc(sizeof(struct conf));
	config->verbosity 		= 0;
	config->publicface		= malloc(128 * sizeof(char));
	config->publicface 		= NULL;
	config->keynode			= malloc(128 * sizeof(char));
	config->keynode 		= NULL;
	opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
	while (opt != -1) {
		switch(opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 'p':
			config->publicface 	= strdup(optarg);
			break;
		case 'k':
			config->keynode 	= strdup(optarg);
			break;
		case 'V':
			config->verbosity++;
			break;
		case 'h':
			print_usage();
			exit(0);
			break;

		case '?':
		case ':':
		default:
			print_usage();
			exit(1);
			break;
		}
		opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
	}
	// maybe use this later to do things with arguments without a flag
	// int i; for(i = optind; i < argc; i++){printf("Do something with: %s\n", argv[i]);}

	init_node();
	free(config);
	return 0;
}
