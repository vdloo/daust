/* init.c */
#include <stdio.h>
#include "messages.h"

void print_version(){
	        printf("daust 0.1\n");
}

void print_usage(){
	printf( "Usage: daust [options]\n"
			"       -V, --version           Print the version and exit.\n"
			"       -h, --help              Print this help and exit.\n\n"
			"Options: \n"
			"  -p, --publicfacing           Specify a public facing address for this node.\n"
			"  -v, --verbose                More verbose output\n"
	      );
}


