#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "net.h"

#define DEFAULT_IRC_PORT "6667"

int main(void)
{
	char *net_name = "erbium.libera.chat";

	struct addrinfo hints = {0}; 

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	struct net_config_t *net_cfg = net_setup(hints, net_name, DEFAULT_IRC_PORT);
	if (net_cfg == NULL) {
		return 1;
	}

	printf("Connecting to %s:%s...\n", net_name, DEFAULT_IRC_PORT);
	int conn_stats = net_connect(net_cfg);
	if (conn_stats < 0) {
		return 1;
	}
	printf("Connected!\n");

	shutdown(net_cfg->sockfd, SHUT_WR);

	char msg[4096] = {0};
	bool end = false;
	while (!end) {
		ssize_t nbuf = recv(net_cfg->sockfd, msg, sizeof msg, 0);
		if (nbuf < 0) {
			perror("Receive data error");
			end = true;
			net_free(net_cfg);
		}

		if (nbuf == 0) {
			end = true;
		}
		printf("%s", msg);
	}

	net_free(net_cfg);
	return 0;
}
