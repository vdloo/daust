/* config.h */
#ifndef CONFIG_H
#define CONFIG_H

struct conf{
	int verbosity;
	int daemon;
	int threadcount;
	int maxthreads;
	int deadend; // daustd instance is passive (doesn't forward anything)
	char *iface;
	char *logfile;
	char *keynode;
	char *publicface;
	char *identifier;
};

char *gen_uuid();
void init_config();
void terminate_config();
void inc_tc();
void dec_tc();
void print_verbose(char *str);

extern struct conf *config;
#endif
