/* config.h */
#ifndef CONFIG_H
#define CONFIG_H
struct conf{
	int verbosity;
	char *publicfacing;
};
extern struct conf *config;
extern void error();
#endif
