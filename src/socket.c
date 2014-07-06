/* socket.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "socket.h"

int server(int portnr)
{
	int sockfd, newsockfd, n, opt=1;
	socklen_t clilen;
	char buffer[512];
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		error("ERROR opening socket");
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portnr);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd,5);
	int z;
	for (z=0;z<5;z++)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd,
				(struct sockaddr *) &cli_addr,
				&clilen);
		if (newsockfd < 0)
			error("ERROR on accept");
		bzero(buffer,512);
		n = read(newsockfd,buffer,511);
		if (n < 0) error("ERROR reading from socket");
		printf("received the following chunk: %s\n",buffer);
		char returnbuffer[] = "chunk successfully reached destination";
		n = write(newsockfd,returnbuffer,strlen(returnbuffer));
		if (n < 0) error("ERROR writing to socket");
	}
	close(newsockfd);
	close(sockfd);
	return 0;
}

int client(int portnr, char buffer[512])
{
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		error("ERROR opening socket");
	}
	char hostN[] = "localhost";
	server = gethostbyname(hostN);
	if (server == NULL){
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
	serv_addr.sin_port = htons(portnr);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("ERROR connecting");
	}
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0){
		error("ERROR writing to socket");
	}
	bzero(buffer,512);
	n = read(sockfd,buffer,511);
	if (n < 0){
		error("ERROR reading from socket");
	}
	printf("%s\n",buffer);
	close(sockfd);
	return 0;
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
