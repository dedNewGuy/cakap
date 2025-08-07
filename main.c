#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_IRC_PORT "6667"

void dump_ipv4_byname(struct addrinfo *info)
{
	char ipstr[INET6_ADDRSTRLEN] = {0};
	struct addrinfo *p = NULL;

	for (p = info; p != NULL; p = p->ai_next) {
		struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
		void *addr = &(ipv4->sin_addr);

		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("    %s: %s\n", "IPV4", ipstr);
	}
}

int main(void)
{
	char *network_name = "erbium.libera.chat";

	struct addrinfo hints = {0}, *serv_info = NULL;

	hints.ai_family = AF_UNSPEC;
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
	printf("Connection will use %d fd to connect throught TCP\n", sockfd);

	printf("Connecting to %s:%s...\n", network_name, DEFAULT_IRC_PORT);
	int conn_stats = connect(sockfd, serv_info->ai_addr, serv_info->ai_addrlen);
	if (conn_stats < 0) {
		perror("Making connection error");
		goto defer;
	}
	printf("Connected!\n");

	shutdown(sockfd, SHUT_WR);

	char msg[4096] = {0};
	bool end = false;
	while (!end) {
		ssize_t nbuf = recv(sockfd, msg, sizeof msg, 0);
		if (nbuf < 0) {
			perror("Receive data error");
			end = true;
			goto defer;
		}

		if (nbuf == 0) {
			end = true;
		}
		printf("%s", msg);
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
