#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include "common_util.h"
#include "net.h"

#define DEFAULT_IRC_PORT "6667"

void *
send_msg_handler(void *args)
{
	struct net_config_t *net_cfg = (struct net_config_t *)args;
	char buffer[4096] = {0};
	while (1) {
		memset(buffer, 0, sizeof buffer);
		char *s = fgets(buffer, sizeof buffer, stdin);
		if (s == NULL) {
			break;
		}
		// APPEND "\r\n" to the buffer: RFC1459
		// https://www.rfc-editor.org/rfc/rfc1459
		int buf_len = strlen(buffer);
		buffer[buf_len - 1] = '\r';
		buffer[buf_len++] = '\n';
		buffer[buf_len] = '\0';
		ssize_t nbuf = send(net_cfg->sockfd, buffer, strlen(buffer), 0);
		if (nbuf < 0) {
			perror("Error sending message");
			continue;
		}

		if (strcmp(buffer, "QUIT\r\n") == 0) break;;
	}

	shutdown(net_cfg->sockfd, SHUT_WR);
	return NULL;
}

void *
recv_msg_handler(void *args)
{
	struct net_config_t *net_cfg = (struct net_config_t *)args;
	char msg[4096] = {0};
	while (1) {
		memset(msg, 0, sizeof msg);
		ssize_t nbuf = recv(net_cfg->sockfd, msg, sizeof msg, 0);
		if (nbuf < 0) {
			perror("Receive data error");
			continue;
		}

		if (nbuf == 0) break;
		printf("%s", msg);

		str_trim_end(strlen(msg), msg);
		char *resp = strtok(msg, " ");
		if (strcmp(resp, "PING") == 0) {
			if (net_pong(*net_cfg, strtok(NULL, " ")) < 0) {
				perror("Failed to pong");
			}
		}
	}

	return NULL;
}

void
irc_send_ident(struct net_config_t *cfg,char *pass, char *nick, char *user)
{
	char buf[256] = {0}; 
	sprintf(buf, 
		 "PASS %s\r\nNICK %s\r\nUSER %s localhost localhost :%s\r\n",
		 pass,
		 nick,
		 nick, user
		 );
	printf("%s\n", buf);
	if (send(cfg->sockfd, buf, strlen(buf), 0) < 0) {
		perror("Sending identity information error");
		fprintf(stderr, "Please send it manually\n");
		return;
	}
}

int 
main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: cakap <server name>\n");
		return 2;
	}
	char *net_name = argv[1];

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

	irc_send_ident(net_cfg, "none", "minzo_912", "Amengdv");

	pthread_t recv_thread, send_thread;

	int recv_thread_stats = pthread_create(&recv_thread, NULL, recv_msg_handler, net_cfg);
	if (recv_thread_stats != 0) {
		perror("error creating receive thread");
		net_free(net_cfg);
		return 1;
	}

	int send_thread_stats = pthread_create(&send_thread, NULL, send_msg_handler, net_cfg);
	if (send_thread_stats != 0) {
		perror("error creating send thread");
		net_free(net_cfg);
		return 1;
	}

	pthread_join(recv_thread, NULL);
	pthread_join(send_thread, NULL);
	net_free(net_cfg);
	return 0;
}
