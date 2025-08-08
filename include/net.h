#ifndef NET_H_
#define NET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define NET_NAME_MAX_LENGTH 256
#define NET_PORT_MAX_LENGTH 16

struct net_config_t {
	struct addrinfo *net_info;
	char net_name[NET_NAME_MAX_LENGTH];
	char net_port[NET_PORT_MAX_LENGTH];
	int sockfd;
};

struct net_config_t*
net_setup(struct addrinfo hints, char *net_name, char *port);

int
net_pong(struct net_config_t cfg, char *message);

int
net_connect(struct net_config_t *cfg);

void
net_free(struct net_config_t *net_config);


#endif // NET_H_
