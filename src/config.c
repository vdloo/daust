#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"

void error(const char *msg)
{
	perror(msg);
}

char *gen_ident() 
{
	char *buf;
	int sz  = 512;
	buf     = malloc(sz * sizeof(char));
	srand(time(NULL));
	--sz;
	buf[sz] = '\0';
	static const char an[] =
		"0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	while (sz > -1) {
		buf[sz] = an[rand() % (sizeof(an) - 1)];
		--sz;
	}
	return buf;
}

void *init_config()
{
	config	= malloc(sizeof(struct conf));
	config->verbosity               = 0;
	config->publicface              = NULL;
	config->keynode                 = NULL;
	config->publicface              = NULL;
	config->identifier 		= gen_ident();
	config->server 			= 0;
	return config;
}

void *terminate_config()
{
	if (config) {
		if (config->keynode) 		free(config->keynode);
		if (config->logfile) 		free(config->logfile);
		if (config->publicface) 	free(config->publicface);
		if (config->identifier)		free(config->identifier);
		free(config);
	}
}
