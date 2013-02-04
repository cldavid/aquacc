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

#define SET_TIME_INTERVAL		60
#define DOSINGUNIT_DEV        	"/dev/ttyACM0"
//#define DOSINGUNIT_DEV		"/dev/sahUSB0"
#define printd					printf
#define SOCKET_PORT             5000 
#define MAXMSG                  8192

#define MAX_SOCKETS				10
typedef struct {
	unsigned int	id;
	unsigned int	free;
	int				fd;
	unsigned int	has_r_data;
	unsigned int	has_w_data;
	char 			r_buffer[MAXMSG];
	char 			w_buffer[MAXMSG];
	size_t			r_len;
	size_t			w_len;
} aq_socket_t;
aq_socket_t	socks[MAX_SOCKETS];

unsigned 	int alive			= 1;
extern 		int errno;

ssize_t writen_ni(int fd, const void *buff, size_t n) {
	size_t nl;
	ssize_t nw;
	const char *p;

	p = buff;
	nl = n;
	while (nl > 0) {
		do {
			nw = write(fd, p, nl);
		} while ( nw < 0 && errno == EINTR );
		if ( nw <= 0 ) break;
		nl -= nw;
		p += nw;
	}
	return n - nl;
}

int is_valid_fd(int fd) {
	int res = fcntl(fd, F_GETFL);
	return res != -1 || errno != EBADF;
}

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
	size_t              size;
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

ssize_t dosing_unit_readH(int fd, char *buffer, ssize_t size) {
	ssize_t	nbytes	= 0;

	do {
		nbytes = read(fd, buffer, size-1);
	} while (nbytes < 0 && errno == EINTR);
	buffer[nbytes] = '\0';
	printf("%s", buffer);
	return nbytes;
}

int openSerial(const char *port, unsigned int bits, unsigned int stop, char parity) {
	struct termios 	tty;
	int 		fd 	= open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if (0 > fd) {
		printd("Error opening serial port %s\n", port);
		alive = 0;
	}

	/*
	 * Get the current tty for the port...
	 */
	memset(&tty, 0, sizeof(tty));
	tcgetattr(fd, &tty);
	cfmakeraw(&tty);

	/*
	 * Set the baud rates to 19200...
	 */
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	/*
	 * Enable the receiver and set local mode...
	 */
	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag |= CLOCAL | CREAD;
	tty.c_cflag &= ~HUPCL;          

	tty.c_cflag |= CRTSCTS;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); 

	tty.c_cc[VMIN]  = 1;
	tty.c_cc[VTIME] = 5;
	/*
	 * Set the new tty for the port...
	 */
	tcsetattr(fd, TCSANOW, &tty);
	tcflush(fd, TCIOFLUSH);
	return fd;
}

ssize_t	setUnixTime(int fd, time_t cur_time) {
	char 	string[250];
	int		len;
	len = snprintf(string, sizeof(string), "setUnixTime %d\n", cur_time);
	return(writen_ni(fd, string, len));
}

void init_sockets(void) {
	memset(socks, 0, sizeof(socks));
	return;
}

int add_socket(int fd) {
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

int free_socket(int fd) {
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

int write_socket(int fd) {
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

int read_socket(int fd) {
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

int daemonize(void) {
	pid_t   pid, sid;

	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(0);
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	if ((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}

	syslog(LOG_NOTICE, "aquacc started by User %d", getuid ());
	return 0;
}

int main(int argc, char *argv[], char *envp[]) {
	fd_set              read_fds;
	fd_set 	        	read_fd_set;
	fd_set				write_fds;
	fd_set	        	write_fd_set;
	size_t				i;
	int 	        	fd_dosing 	= openSerial(DOSINGUNIT_DEV, 8, 1, 'N');
	int                 fd_socket   = makeSocket(SOCKET_PORT);
	int		        	fd			= -1;
	int                 new_fd      = -1;
	int		        	maxfd		= FD_SETSIZE;
	int                 sres		= -1;
	time_t	        	cur_time;
	time_t				prev_time;
	char 				dos_buffer[MAXMSG];
	ssize_t				dos_len;
	struct timeval 		stimeout;


	//daemonize();

	init_sockets();
	if (listen(fd_socket, 1) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	time(&cur_time);
	prev_time = cur_time;

	FD_ZERO(&read_fd_set);
	FD_ZERO(&write_fd_set);
	FD_SET(fd_socket, &read_fd_set);
	FD_SET(fd_dosing, &read_fd_set);
	while (alive && is_valid_fd(fd_dosing) && is_valid_fd(fd_socket)) {
		read_fds 	= read_fd_set;	
		write_fds	= write_fd_set;

		stimeout.tv_sec		= 1;
		stimeout.tv_usec	= 0;
		if (0 > (sres = select(maxfd, &read_fds, &write_fds,  NULL, &stimeout))) {
			if (errno != EINTR) {
				perror("select");
				goto exit;
			}	
		}

		time(&cur_time);
		for (fd = 0; fd < maxfd; fd++) {
			if (FD_ISSET(fd, &read_fds)) {
				/* Read From Dosing Unit */
				if (fd == fd_dosing) {
					dos_len = dosing_unit_readH(fd, dos_buffer, sizeof(dos_buffer));
					if (dos_len >= 0) {
						/* Copy Buffer To All Connected Sockets */
						for (i = 0; i < MAX_SOCKETS; i++) {
							if (socks[i].free) {
								if (socks[i].has_w_data) {
									printf("Appending wlen %d + dos_len %d\n", socks[i].w_len, dos_len);
									memcpy(socks[i].w_buffer + socks[i].w_len, dos_buffer, dos_len);
									socks[i].has_w_data = 1;
									socks[i].w_len += dos_len;
								} else {
									memcpy(socks[i].w_buffer, dos_buffer, dos_len);
									socks[i].has_w_data = 1;
									socks[i].w_len = dos_len;
								}
								FD_SET(socks[i].fd, &write_fd_set);
							}
						}
					}
					/* Handle Ne Socket Connection */
				} else if (fd == fd_socket) {
					if (0 < (new_fd = acceptSocket(fd))) {
						if (0 > add_socket(new_fd)) {
							writen_ni(new_fd, "Max Sockets", strlen("Max Sockets"));
							close(new_fd);
							continue;
						}
						FD_SET(new_fd, &read_fd_set);
					}
					/* Read From Current Socket */
				} else {
					if (0 >= read_socket(fd)) {
						fprintf(stderr, "Error: reading from socket.\nClosing socket %d.\n", fd);
						free_socket(fd);
						FD_CLR(fd, &read_fd_set);
						close(fd);
						continue;
					} 
					FD_SET(fd_dosing, &write_fd_set);
				}
			} else if (FD_ISSET(fd, &write_fds)) {
				if (fd == fd_dosing) {
					/* Write socket information to dosing unit */
					for (i = 0; i < MAX_SOCKETS; i++) {
						if (socks[i].free && socks[i].has_r_data) {
							writen_ni(fd_dosing, socks[i].r_buffer, socks[i].r_len);
							socks[i].has_r_data = 0;
							socks[i].r_len = 0;
						}
					}
					FD_CLR(fd, &write_fd_set);
				} else if (fd != fd_socket) {
					write_socket(fd);
					FD_CLR(fd, &write_fd_set);
				} 
			}
		}
		/* Write current the current time to the dosing unit */
		if ((cur_time - prev_time) >= SET_TIME_INTERVAL) {
			setUnixTime(fd_dosing, cur_time);
			prev_time = cur_time;
		}
	}

exit:
	/* Cleanup */
	for (i = 0; i < MAX_SOCKETS; i++) {
		free_socket(socks[i].fd);
		close(socks[i].fd);
	}
	close(fd_dosing);
	close(fd_socket);
	exit(EXIT_SUCCESS);
}
