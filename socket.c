#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "aquacc.h"
#include "socket.h"

aq_socket_t	socks[MAX_SOCKETS];
extern 		int errno;

int makeSocket (int16_t port) {
	int	on		= 1;
	int sock;
	struct sockaddr_in name;

	/* Create the socket. */
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	/* Give the socket a name. */
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	return sock;
}

/* Connection request on original socket. */
int acceptSocket(int sock) {
	int                 new;
	socklen_t           size;
	struct sockaddr_in  clientname;
	char                *host;
	uint16_t            port;

	size = sizeof(clientname);
	new = accept(sock, (struct sockaddr *)&clientname, &size);
	if (new < 0) {
		perror("accept");
		return -1;
	}
	host = inet_ntoa(clientname.sin_addr);
	port = ntohs(clientname.sin_port);
	fprintf(stderr, "Server: connect from host %s, port %u.\n", host, port);
	return new;
}

void initSocket(void) {
	memset(socks, 0, sizeof(socks));
	return;
}

int addSocket(int fd) {
	size_t i;

	for (i = 0; i < MAX_SOCKETS; i++) {
		if (!socks[i].free) {
			socks[i].id			= i;
			socks[i].free 		= 1;
			socks[i].fd   		= fd;
			socks[i].has_w_data = 0;
			socks[i].has_r_data = 0;
			socks[i].r_len      = 0;
			socks[i].w_len      = 0;
			return i;
		}
	}
	fprintf(stderr, "Error: No free socket found\n");
	return -1;
}

int freeSocket(int fd) {
	size_t i;

	for (i = 0; i < MAX_SOCKETS; i++) {
		if (socks[i].fd == fd) {
			socks[i].free 		= 0;
			socks[i].fd   		= 0;
			socks[i].has_w_data = 0;
			socks[i].has_r_data = 0;
			socks[i].r_len 		= 0;
			socks[i].w_len 		= 0;
			return i;
		}
	}
	return -1;
}

int writeSocket(int fd) {
	int		ret;
	size_t  i;

	for (i = 0; i < MAX_SOCKETS; i++) {
		if ((socks[i].free) && (socks[i].fd == fd)) {
			ret = writen_ni(socks[i].fd, socks[i].w_buffer, socks[i].w_len);
			socks[i].has_w_data = 0;
			socks[i].w_len = 0;
			return ret;
		}
	}
	return -1;
}

int readSocket(int fd) {
	int 	nbytes = 0;
	size_t 	i;

	for (i = 0; i < MAX_SOCKETS; i++) {
		if ((socks[i].free) && (socks[i].fd == fd)) {
			do {
				if (socks[i].has_r_data) {
					nbytes = read(socks[i].fd, 
							socks[i].r_buffer + socks[i].r_len,  
							sizeof(socks[i].r_buffer) - socks[i].r_len);
					socks[i].r_len += nbytes;
				} else {
					nbytes = read(socks[i].fd, socks[i].r_buffer, sizeof(socks[i].r_buffer));
					socks[i].r_len = nbytes;
				}
			} while (nbytes < 0 && errno == EINTR);
			socks[i].r_buffer[nbytes] = '\0';  
			socks[i].has_r_data = 1;
			return nbytes;
		}
	}
	return -1;
}

int listenSocket(int sockfd, int backlog) {
    return(listen(sockfd, backlog));
}

int closeSocket(int fd) {
    return(close(fd));
}
