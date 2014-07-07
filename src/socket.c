/* socket.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "socket.h"

char *hostname(){
	char *hn = malloc(128 * sizeof(char));
	gethostname(hn, 128 * sizeof(char));
	return hn;
}

char *internalhost(){
	struct ifreq ifr;
	int sh = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	ioctl(sh, SIOCGIFADDR, &ifr);
	close(sh);
	char *iaddr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
	return iaddr;
}
