#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "net.h"

#define DEFAULT_IRC_PORT "6667"

void *
recv_msg_handler(void *args)
{
	struct net_config_t *net_cfg = (struct net_config_t *)args;
	char msg[4096] = {0};
	bool end = false;
	while (!end) {
		memset(msg, 0, sizeof msg);
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
	
	return NULL;
}

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

	pthread_t recv_thread;
	int recv_thread_stats = pthread_create(&recv_thread, NULL, recv_msg_handler, net_cfg);
	if (recv_thread_stats != 0) {
		perror("error creating receive thread");
		net_free(net_cfg);
		return 1;
	}

	pthread_join(recv_thread, NULL);
	net_free(net_cfg);
	return 0;
}
