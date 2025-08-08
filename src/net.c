#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "net.h"

struct net_config_t* 
net_setup(struct addrinfo hints, char *net_name, char *port)
{
	struct addrinfo *serv_info = NULL;

	int addrinfo_stats = getaddrinfo(net_name, port, &hints, &serv_info);
	if (addrinfo_stats < 0) {
		fprintf(stderr, "net_setup: getaddrinfo: %s\n", gai_strerror(addrinfo_stats));
		return NULL;
	}

	int sockfd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
	if (sockfd < 0) {
		perror("net_setup: creating socket error");
		goto defer;
	}

	struct net_config_t *net_cfg = malloc(sizeof(struct net_config_t));
	if (net_cfg == NULL) {
		perror("net_setup: malloc:");
		goto defer;
	}

	net_cfg->net_info = serv_info;
	if (net_name != NULL)
		strcpy(net_cfg->net_name, net_name);
	if (port != NULL)
		strcpy(net_cfg->net_port, port);
	net_cfg->sockfd = sockfd;

	return net_cfg;

defer:
	freeaddrinfo(serv_info);
	if (sockfd != -1) {
		close(sockfd);
	}
	return NULL;
}

int
net_connect(struct net_config_t *cfg)
{
	int conn_stats = connect(cfg->sockfd, cfg->net_info->ai_addr, cfg->net_info->ai_addrlen);
	if (conn_stats < 0) {
		perror("Making connection error");
		net_free(cfg);
		return conn_stats;
	}

	return conn_stats;
}

void
net_free(struct net_config_t *net_config)
{
	freeaddrinfo(net_config->net_info);
	close(net_config->sockfd);
	free(net_config);
}

// void dump_ip_byname(struct addrinfo *info)
// {
// 	char ipstr[INET6_ADDRSTRLEN] = {0};
// 	struct addrinfo *p = NULL;
//
// 	for (p = info; p != NULL; p = p->ai_next) {
// 		void *addr;
// 		char *ipver;
// 		struct sockaddr_in *ipv4;
// 		struct sockaddr_in6 *ipv6;
//
// 		if (p->ai_family == AF_INET) {
// 			ipv4 = (struct sockaddr_in *) p->ai_addr;
// 			addr = &(ipv4->sin_addr);
// 			ipver = "IPv4";
// 		} else {
// 			ipv6 = (struct sockaddr_in6 *) p->ai_addr;
// 			addr = &(ipv6->sin6_addr);
// 			ipver = "IPv6";
// 		}
//
// 		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
// 		printf("    %s: %s\n", ipver, ipstr);
// 	}
// }
