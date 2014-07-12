/* config.h */
#ifndef CONFIG_H
#define CONFIG_H

struct conf{
	int verbosity;
	int server; // replace this with threading
	char *logfile;
	char *keynode;
	char *publicface;
	char *identifier;
};

void error(const char *msg);
void *init_config();
void *terminate_config();

extern struct conf *config;
#endif
