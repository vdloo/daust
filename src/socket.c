/* socket.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "socket.h"

#define MAX_DATA_LENGTH 8189

struct pack {
	unsigned short dli;
	char *res, *d;
};

unsigned short get_dli(char *res) 
{
	int ln = strlen(res); 
	return ln < MAX_DATA_LENGTH ? ln : MAX_DATA_LENGTH;
}

void forge_packet(struct pack *pkt)
{
//	if (pkt->d) free(pkt->d);
	pkt->d = malloc(sizeof(unsigned short) + pkt->dli * sizeof(char) + 1);
	memcpy(pkt->d, &pkt->dli, sizeof(unsigned short));
	memcpy(pkt->d + sizeof(unsigned short), pkt->res, pkt->dli);
	char nt = '\0';
	memcpy(pkt->d + MAX_DATA_LENGTH + 2, &nt, 1);
}

int save_residu(struct pack *pkt)
{
	char *tmp;
	int nln = strlen(pkt->res) - pkt->dli;
	if (nln > 0) {
		tmp = malloc(nln);
		memcpy(tmp, pkt->res + pkt->dli, nln); //deze gaat fout
		free(pkt->res);
		if (tmp) pkt->res = strdup(tmp);
		free(tmp);
	}
	return nln > 0;
}

int send_packets(char *host, int port, char *buf)
{
	int sh, n;
	struct sockaddr_in sa;
	struct hostent *serv;
	sh = socket(AF_INET, SOCK_STREAM, 0);
	if (sh < 0) {
		error("ERROR opening socket");
	}
	sa.sin_addr.s_addr 	= inet_addr("172.28.128.1"); 
	if (sa.sin_addr.s_addr == INADDR_NONE) {
		printf("WARNING invalid address\n");
	}
	sa.sin_family 		= AF_INET;
	sa.sin_port 		= htons(port);
	if (connect(sh, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		error("Error connecting");
	}

	struct pack *pkt = malloc(sizeof(struct pack));
	pkt->res = buf;
	pkt->d = NULL;
	do {
		pkt->dli = get_dli(pkt->res);
		forge_packet(pkt);

		n = write(sh, pkt->d, pkt->dli + 2);
		if (n < 0) {
			error("Error writing to socket");
		}
	} while(save_residu(pkt));

	close(sh);
	return 0;
}

int receive_packets(int port)
{
	int sh, nsh, n, o = 1;
	socklen_t cl;
	struct sockaddr_in sa, cla;
	sh = socket(AF_INET, SOCK_STREAM, 0);
	if (sh < 0) {
		error("ERROR opening socket");
	}
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(port);
	setsockopt(sh, SOL_SOCKET, SO_REUSEADDR, (const char *) &o, sizeof(int));
	if (bind(sh, (struct sockaddr *) &sa, sizeof(sa)) > 0) {
		error("ERROR on binding");
	}
	listen(sh, 5);
	int i;
	for (i = 0; i < 5; i++) {
		cl = sizeof(cla);
		nsh = accept(sh, (struct sockaddr *) &cla, &cl);
		if (nsh < 0) {
			error("ERROR accepting socket");
		}
		unsigned short dli;
		n = read(nsh, dli, sizeof(unsigned short));
		if (n < 0) error("ERROR reading data length from socket");
		printf("received unsigned double is %hu\n", dli);
	}
	close(nsh);
	close(sh);
	return 0;
}

char *hostname()
{
	char *hn = malloc(128 * sizeof(char));
	gethostname(hn, 128 * sizeof(char));
	return hn;
}

char *internalhost()
{
	struct ifreq ifr;
	int sh = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eth1", IFNAMSIZ-1);
	ioctl(sh, SIOCGIFADDR, &ifr);
	close(sh);
	char *iaddr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
	return iaddr;
}
