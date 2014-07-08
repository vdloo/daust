/* config.h */
#ifndef CONFIG_H
#define CONFIG_H
struct conf{
	int verbosity;
	int server; // replace this with threading
	char *publicface;
	char *keynode;
};
extern struct conf *config;
extern void error();
#endif
