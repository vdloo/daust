/* config.h */
#ifndef CONFIG_H
#define CONFIG_H

struct conf{
	int verbosity;
	int daemon;
	char *logfile;
	char *keynode;
	char *publicface;
	char *identifier;
};

void *init_config();
void *terminate_config();

extern struct conf *config;
#endif
