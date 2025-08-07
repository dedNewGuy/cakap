#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define DEFAULT_IRC_PORT "6667"

int main(void)
{
	char *network_name = "chat.freenode.net";

	struct addrinfo hints = {0}, *serv_info = NULL;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int addrinfo_stats = getaddrinfo(network_name, DEFAULT_IRC_PORT, &hints, &serv_info);
	if (addrinfo_stats < 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addrinfo_stats));
		return 1;
	}

	int sockfd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
	if (sockfd < 0) {
		perror("creating socket error");
		goto defer;
	}

	int conn_stats = connect(sockfd, serv_info->ai_addr, serv_info->ai_addrlen);
	if (conn_stats < 0) {
		perror("Making connection error");
		goto defer;
	}

	freeaddrinfo(serv_info);
	close(sockfd);
	return 0;

defer:
	freeaddrinfo(serv_info);
	if (sockfd != -1) {
		close(sockfd);
	}
	return 1;
}
