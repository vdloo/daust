/* daustd.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "config.h"
#include "messages.h"
#include "init.h"
#include "server.h"

static const char *os = "dhk:l:p:vVi:e";
static const struct option lo[] = {
	{ "daemon", no_argument, NULL, 'd' },
	{ "help", no_argument, NULL, 'h' },
	{ "keynode", required_argument, NULL, 'k' },
	{ "logfile", required_argument, NULL, 'l' },
	{ "publicface", required_argument, NULL, 'p' },
	{ "deadend", required_argument, NULL, 'e' },
	{ "iface", required_argument, NULL, 'i' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "version", no_argument, NULL, 'V' },
	{ NULL, no_argument, NULL, 0 }
};

struct conf *config;

char *check_if_running()
{
	char *r		= NULL;
	char *dest 	= NULL;
	dest		= strdup("127.0.0.1");
	char *cmd	= NULL;
	cmd 		= strdup("ping");
	r = broadcast_command(dest, cmd);
	free(dest);
	free(cmd);
	return r;
}

int main(int argc, char *argv[])
{
	init_config();

	char *r;
	if (r = check_if_running()) {
		free(r);
		printf("daustd is already running\n");
		return 1;
	}

	int opt, li = 0, in = 1;
	opt = getopt_long(argc, argv, os, lo, &li);
	while (opt != -1) {
		switch(opt) {
			case 'd':
				config->daemon++;
				break;
			case 'e':
				config->deadend++;
				break;
			case 'k':
				free(config->keynode);
				config->keynode 	= strdup(optarg);
				break;
			case 'i':
				free(config->iface);
				config->iface 		= strdup(optarg);
				break;
			case 'l':
				free(config->logfile);
				config->logfile 	= strdup(optarg);
				break;
			case 'p':
				free(config->publicface);
				config->publicface 	= strdup(optarg);
				break;
			case 'v':
				config->verbosity++;
				break;
			case 'V':
				print_version();
				break;
			case 'h':
			case '?':
			case ':':
			default:
				print_daustd_usage();
				return 1;
				break;
		}
		opt = getopt_long(argc, argv, os, lo, &li);
	}

	if (config->daemon) {
		if (daemon(0,0) == -1) {
			perror("ERROR detaching");
		}
	}

	init_server();
	terminate_config();
	return 0;
}
