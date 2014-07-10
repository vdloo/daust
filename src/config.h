/* config.h */
#ifndef CONFIG_H
#define CONFIG_H
struct conf{
	int verbosity;
	int server; // replace this with threading
	char *logfile;
	char *keynode;
	char *publicface;
};
extern struct conf *config;
extern void error();
#endif
