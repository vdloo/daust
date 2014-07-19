/* daust.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "config.h"
#include "messages.h"
#include "client_dispatch.h"

static const char *os = "hVv";
static const struct option lo[] = {
	{ "help", no_argument, NULL, 'h' },
	{ "keynode", required_argument, NULL, 'k' },
	{ "logfile", required_argument, NULL, 'l' },
	{ "publicface", required_argument, NULL, 'p' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "version", no_argument, NULL, 'V' },
	{ NULL, no_argument, NULL, 0 }
};

struct conf *config;

int main(int argc, char **argv)
{
	init_config();

	int opt, li = 0;
	opt = getopt_long(argc, argv, os, lo, &li);
	while (opt != -1) {
		switch(opt) {
			case 'v':
				config->verbosity++;
				break;
			case 'V':
				print_version();
				return 0;
				break;
			case 'h':
			case '?':
			case ':':
			default:
				print_daust_usage();
				return 1;
				break;
		}
		opt = getopt_long(argc, argv, os, lo, &li);
	}
	if (optind < argc) {
		client_dispatch(argc, argv, optind);
	} else {
		print_daust_usage();
	}

	terminate_config();
	return 0;
}
