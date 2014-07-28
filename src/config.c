#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "config.h"
#include "node_data.h"

#define PATH_MAX 4096

char *gen_uuid() 
{
	char *buf;
	int sz  = 10;
	buf 	= NULL;
	buf     = malloc(sz * sizeof(char));
	struct timeval t;
	gettimeofday(&t, NULL);
	srand(t.tv_usec * t.tv_sec);
	static const char an[] =
		"0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;
	int j = sz - 1;
	for (i = 0; i < j; ++i) {
		buf[i] = an[rand() % (sizeof(an) - 1)];
	}
	buf[sz - 1] 	= '\0';
	return buf;
}

char *get_ident_path()
{
	char *p 	= malloc(PATH_MAX * sizeof(char));
	char *fn 	= ".daust";
	char *hd	= getenv("HOME");
	char *root	= strdup("/");
	if (strcmp(hd, root) == 0) {
		*hd = '\0'; // remember that it is not a good idea to run this program as root
	}
	free(root);
	snprintf(p, PATH_MAX, "%s/%s", hd, fn);
	mkdir(p, 0777);
	snprintf(p, PATH_MAX, "%s/%s/%s", hd, fn, "ident");
	return p;
}

char *read_ident()
{
	FILE *f;
	char *p = get_ident_path();
	f = fopen(p, "r");
	free(p);
	char *buf;
	buf = NULL;
	if (f != NULL) {
		fseek(f, 0, SEEK_END);
		long fs = ftell(f);
		fs = ftell(f);
		rewind(f);	
		buf = malloc((1 + fs) * (sizeof(char))); // + 1 for nullterm
		size_t r = fread(buf, sizeof(char), fs, f);
		fclose(f);
		buf[fs] = '\0';
	}
	return buf;
}

char *write_ident(char *buf)
{
	FILE *f;
	char *p = get_ident_path();
	f = fopen(p, "w");
	free(p);
	if (f != NULL) {
		fputs(buf, f);
		fclose(f);
	}
}

void *new_ident()
{
	char *buf;
	buf = NULL;
	buf = gen_uuid();
	write_ident(buf);
	return buf;
}

char *get_ident()
{
	char *buf;
	buf = read_ident();
	if (!buf) {
		buf = new_ident();
	}
	return buf;
}

void init_config()
{
	config = NULL;
	config = malloc(sizeof(struct conf));
	config->daemon 			= 0;
	config->deadend			= 0;
	config->threadcount		= 1;
	config->maxthreads		= 8;
	config->iface			= strdup("eth0");
	config->identifier 		= get_ident();
	config->keynode                 = strdup(na);
	config->publicface              = strdup(na);
	config->verbosity               = 0;
	config->logfile			= strdup(na);
}

void terminate_config()
{
	if (config) {
		if (config->identifier)		free(config->identifier);
		if (config->keynode) 		free(config->keynode);
		if (config->logfile) 		free(config->logfile);
		if (config->publicface) 	free(config->publicface);
		if (config->iface) 		free(config->iface);
		free(config);
	}
}

void print_inc_tc()
{
	if (config->verbosity) {
//		printf("Increased threadcount to %d\n", config->threadcount);
	}
}

void inc_tc()
{
	++config->threadcount;
	print_inc_tc();
}

void print_dec_tc()
{
	if (config->verbosity) {
//		printf("Decreased threadcount to %d\n", config->threadcount);
	}
}

void dec_tc()
{
	--config->threadcount;
	print_dec_tc();
}
