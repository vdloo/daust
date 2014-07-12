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
	{ "daemon", no_argument, NULL, 'd' },
	{ "help", no_argument, NULL, 'h' },
	{ "keynode", required_argument, NULL, 'k' },
	{ "logfile", required_argument, NULL, 'l' },
	{ "publicface", required_argument, NULL, 'p' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "version", no_argument, NULL, 'V' },
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
		case 'd': //replace this with threading
			config->daemon++;
			break;
		case 'h':
			print_usage();
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
		case 'v':
			config->verbosity++;
			break;
		case 'V':
			print_version();
			--in;
			break;

		case '?':
		case ':':
		default:
			print_usage();
			--in;
			break;
		}
		opt = getopt_long(argc, argv, os, lo, &li);
	}

	if (in > 0) {
		init_nodelist();
		init_dispatch(argc, argv, optind);

		terminate_nodelist();
	}

	terminate_config();

	return 0;
}
