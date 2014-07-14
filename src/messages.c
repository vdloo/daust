/* messages.c */
#include <stdio.h>
#include "messages.h"

void print_version()
{
	printf("daust 0.1\n");
}

void print_daust_usage()
{
	printf( "Usage: daust [options] [commands]\n"
		"\t-V, --version\t\tPrint the version and exit.\n"
		"\t-h, --help\t\tPrint this help and exit.\n\n"
		"Options: \n"
		"\t-v, --verbose\t\tMore verbose output\n\n"
		"Commands: \n"
		"\tping\t\t\tSee if node is up\n"
		"\tlist\t\t\tList known nodes\n"
		"\tstop\t\t\tKill the daustd process\n\n"
		"Examples: \n"
		"\tdaust list\t\tList locally known nodes\n"
		"\tdaust remote node1 ping\tPing daustd on node1\n"
		"\tdaust all stop\t\tKill daustd on all nodes\n"
	);
}

void print_daustd_usage()
{
	printf( "Usage: daust [options]\n"
		"\t-V, --version\t\tPrint the version and exit.\n"
		"\t-h, --help\t\tPrint this help and exit.\n\n"
		"Options: \n"
		"\t-l, --logfile\t\tFilename for optional logging.\n"
		"\t-k, --keynode\t\tNode that will introduce this node to the network.\n"
		"\t-p, --publicfacing\tSpecify a public facing address for this node.\n"
		"\t-v, --verbose\t\tMore verbose output\n"
	);
}


