/* socket.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "socket.h"
#include "config.h"

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
	if (pkt->d) free(pkt->d);
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
		memcpy(tmp, pkt->res + pkt->dli, nln);
		free(pkt->res);
		if (tmp) pkt->res = strdup(tmp);
		free(tmp);
	}
	return nln > 0;
}

char *send_packets(char *host, int port, char *buf, char *(*cb)(char *param))
{
	int sh, n;
	struct sockaddr_in sa;
	struct hostent *serv;
	sh = socket(AF_INET, SOCK_STREAM, 0);
	if (sh < 0) {
		if (config->verbosity) {
			perror("ERROR opening socket");
		}
		return NULL;
	}
	sa.sin_addr.s_addr 	= inet_addr(host);
	if (sa.sin_addr.s_addr == INADDR_NONE) {
		if (config->verbosity) {
			perror("ERROR invalid address");
		}
		return NULL;
	}
	sa.sin_family 		= AF_INET;
	sa.sin_port 		= htons(port);
	if (connect(sh, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		if (config->verbosity) {
			perror("ERROR connecting");
		}
		return NULL;
	}

	struct pack *pkt = malloc(sizeof(struct pack));
	pkt->res = buf;
	pkt->d = NULL;
	do {
		pkt->dli = get_dli(pkt->res);
		forge_packet(pkt);

		n = write(sh, pkt->d, pkt->dli + 2);
		if (n < 0) {
			if (config->verbosity) {
				perror("ERROR writing to socket");
			}
			return NULL;
		}
	} while(save_residu(pkt));

	// catch return packet
	unsigned short dli = 0, prev_dli = 0;
	int m_siz = 0;
	char *rbuf = NULL;
	do {
		n = read(sh, &dli, sizeof(unsigned short));
		if (n < 0) {
			if (config->verbosity) {
				perror("ERROR reading data length from socket");
			}
		}
		m_siz = m_siz + (dli * sizeof(char));
		rbuf = realloc(rbuf, m_siz);
		n = read(sh, rbuf + prev_dli, (dli * sizeof(char)));
		prev_dli = prev_dli + dli;
	} while (dli == MAX_DATA_LENGTH);
	rbuf[m_siz] 	= '\0';
	char *res 	= "no response data";
	if (n > 0) {
		res 	= cb(rbuf);
	} else {
		if (config->verbosity) {
			perror("ERROR connection sent no response\n");
		}
	}

	close(sh);
	return res;
}

void process_incoming(int sh, char *(*cb)(char *param))
{
	int nsh, n;
	struct sockaddr_in cla;
	socklen_t cl = sizeof(cla);
	nsh = accept(sh, (struct sockaddr *) &cla, &cl);
	if (nsh < 0) {
		if (config->verbosity) {
			perror("ERROR accepting socket");
		}
	}
	unsigned short dli = 0, prev_dli = 0;
	int m_siz = 0;
	char *buf = NULL;
	do {
		n = read(nsh, &dli, sizeof(unsigned short));
		if (n < 0) {
			perror("ERROR reading data length from socket");
		}
		m_siz = m_siz + (dli * sizeof(char));
		buf = realloc(buf, m_siz);
		n = read(nsh, buf + prev_dli, (dli * sizeof(char)));
		prev_dli = prev_dli + dli;
	} while (dli == MAX_DATA_LENGTH);
	buf[m_siz] = '\0';

	// send return packet
	char *rbuf = NULL;
	rbuf = cb(buf);
	struct pack *pkt = malloc(sizeof(struct pack));
	pkt->res = rbuf;
	pkt->d = NULL;
	do {
		pkt->dli = get_dli(pkt->res);
		forge_packet(pkt);

		n = write(nsh, pkt->d, pkt->dli + 2);
		if (n < 0) {
			if (config->verbosity) {
				perror("ERROR writing to socket");
			}
		}
	} while(save_residu(pkt));

	close(nsh);
	free(buf);
}


int receive_packets(int port, char *(*cb)(char *param))
{
	int sh, o = 1;
	socklen_t cl;
	struct sockaddr_in sa;
	sh = socket(AF_INET, SOCK_STREAM, 0);
	if (sh < 0) {
		perror("ERROR opening socket");
	}
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(port);
	setsockopt(sh, SOL_SOCKET, SO_REUSEADDR, 
			(const char *) &o, sizeof(int));
	if (bind(sh, (struct sockaddr *) &sa, sizeof(sa)) > 0) {
		perror("ERROR on binding");
	}
	listen(sh, 5);
	while(1) process_incoming(sh, cb);
	close(sh);
	return 0;
}

// writes hostname to a static buffer that gets overwritten every
// time the function is called, returns pointer to this buffer
char *hn;
char *hostname()
{
	hn = malloc(128 * sizeof(char));
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
