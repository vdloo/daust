/* main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "config.h"
#include "messages.h"
#include "init.h"

static const char *os = "vk:l:p:Vsh";
static const struct option lo[] = {
	{ "version", no_argument, NULL, 'v' },
	{ "keynode", required_argument, NULL, 'k' },
	{ "logfile", required_argument, NULL, 'l' },
	{ "publicface", required_argument, NULL, 'p' },
	{ "verbose", no_argument, NULL, 'V' },
	{ "server", no_argument, NULL, 's' }, //replace this with threading
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};

struct conf *config;

int main(int argc, char *argv[])
{
	init_config();

	int opt, li = 0, in = 1;
	opt = getopt_long(argc, argv, os, lo, &li);
	while (opt != -1) {
		switch(opt) {
		case 'v':
			print_version();
			--in;
			break;
		case 'k':
			config->keynode 	= strdup(optarg);
			break;
		case 'l':
			config->logfile 	= strdup(optarg);
			break;
		case 'p':
			config->publicface 	= strdup(optarg);
			break;
		case 'V':
			config->verbosity++;
			break;
		case 's': //replace this with threading
			config->server++;
			break;
		case 'h':
			print_usage();
			--in;
			break;

		case '?':
		case ':':
		default:
			print_usage();
			exit(1);
			break;
		}
		opt = getopt_long(argc, argv, os, lo, &li);
	}

	if (in) {
		init_nodelist();
		init_dispatch(argc, argv, optind);

		terminate_nodelist();
	}

	terminate_config();

	return 0;
}
