/* config.h */
#ifndef CONFIG_H
#define CONFIG_H

struct conf{
	int verbosity;
	int daemon;
	int threadcount;
	int maxthreads;
	char *logfile;
	char *keynode;
	char *publicface;
	char *identifier;
};

void init_config();
void terminate_config();
void inc_tc();
void dec_tc();

extern struct conf *config;
#endif
