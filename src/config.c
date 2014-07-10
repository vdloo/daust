#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"

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
