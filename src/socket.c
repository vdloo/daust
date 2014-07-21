/* socket.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pthread.h> 

#include "socket.h"
#include "config.h"

#define MAX_DATA_LENGTH 8189

struct pack {
	unsigned short dli;
	char *res, *d;
};

unsigned short get_dli(char *res) 
{
	if (res) {
		int ln = strlen(res); 
		return ln < MAX_DATA_LENGTH ? ln : MAX_DATA_LENGTH;
	} else {
		return 0;
	}
}

void forge_packet(struct pack *pkt)
{
	if (pkt) {
		if (pkt->d) free(pkt->d);
		pkt->d = malloc(sizeof(unsigned short) + pkt->dli * sizeof(char) + 1);
		memcpy(pkt->d, &pkt->dli, sizeof(unsigned short));
		memcpy(pkt->d + sizeof(unsigned short), pkt->res, pkt->dli);
		char nt = '\0';
		memcpy(pkt->d + sizeof(unsigned short) + pkt->dli, &nt, 1);
	}
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
	pkt->res = NULL;
	pkt->res = buf;
	pkt->d = NULL;
	if (pkt) {
		do {
			pkt->dli = get_dli(pkt->res);
			forge_packet(pkt);

			if (pkt->d && pkt->dli) {
				n = write(sh, pkt->d, pkt->dli + 2);
				if (n < 0) {
					if (config->verbosity) {
						perror("ERROR writing to socket");
					}
					return NULL;
				}
			}
		} while(save_residu(pkt));
	}
	if (pkt->d) free(pkt->d);
	if (pkt) free(pkt);

	// catch return packet
	unsigned short dli = 0, prev_dli = 0;
	int m_siz = 1;
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
	rbuf[m_siz - 1] 	= '\0';
	char *res 	= NULL;
	if (n > 0) {
		res 	= cb(rbuf);
	} else {
		res 	= strdup("Server closed the connection. Goodbye");
	}
	if (rbuf) free(rbuf);
	close(sh);
	return res;
}

struct thread_args
{
	int sh;
	char *(*cb)(char *param);
};

void process_incoming(void *ta)
{
	struct thread_args *d;
	d = (struct thread_args *) ta;
	int nsh = d->sh;
	char *(*cb)(char *param) = d->cb;

	unsigned short dli = 0, prev_dli = 0;
	int n;
	int m_siz = 1;
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
	buf[m_siz - 1] = '\0';

	// send return packet
	char *rbuf = NULL;
	rbuf = cb(buf);

	if (buf) free(buf);

	struct pack *pkt = malloc(sizeof(struct pack));
	pkt->res = rbuf;
	pkt->d = NULL;
	if (pkt && pkt->res) {
		do {
			pkt->dli = get_dli(pkt->res);
			forge_packet(pkt);

			if (pkt->d && pkt->dli) {
				n = write(nsh, pkt->d, pkt->dli + 2);
				if (n < 0) {
					if (config->verbosity) {
						perror("ERROR writing to socket");
					}
				}
			}
		} while(save_residu(pkt));
	}
	if (pkt->d) free(pkt->d);
	if (pkt) free(pkt);
	if (rbuf) free(rbuf);
	close(nsh);
	dec_tc();
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
	pthread_t nt;
	struct thread_args ta;
	while(1) {
		int nsh;
		struct sockaddr_in cla;
		socklen_t cl = sizeof(cla);
		printf("waiting for accept\n");
		nsh = accept(sh, (struct sockaddr *) &cla, &cl);
		printf("accepted\n");
		if (nsh < 0) {
			if (config->verbosity) {
				perror("ERROR accepting socket");
			}
		}

		ta.sh = nsh;
		ta.cb = cb;
		while (config->threadcount > config->maxthreads) {
			sleep(1);
		}
		inc_tc();
		printf("creating thread\n");
		pthread_create(&nt, NULL, (void *) &process_incoming, (void *) &ta);
		printf("continued after thread\n");
	}
	close(sh);
	return 0;
}

char *hostname()
{
	char *hn;
	hn = NULL;
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
	return strdup(iaddr);
}
