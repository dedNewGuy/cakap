#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

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

	freeaddrinfo(serv_info);
	return 0;
}
