/*
 * This file is part of Aquarium Control Center (aquacc).
 *
 * Copyright (C) 2012-2013 Cluytens David <david.cluytens@gmail.com>
 *
 * Aquarium Control Center (aquacc) is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, 
 * or (at your option) any later version.
 *
 * Aquarium Control Center (aquacc) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Aquarium Control Center (aquacc). If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "aquacc.h"
#include "serial.h"
#include "socket.h"
#include "daemon.h"
#include "dsu.h"

extern aq_socket_t	socks[MAX_SOCKETS];

unsigned 	int alive			= 1;
extern 		int errno;

ssize_t	setUnixTime(int fd, time_t cur_time) {
	char 	string[250];
	int		len;
	len = snprintf(string, sizeof(string), "setUnixTime %lu\n", cur_time);
	return(writen_ni(fd, string, len));
}

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)), char *envp[] __attribute__ ((unused))) {
	fd_set              read_fds;
	fd_set 	       	    read_fd_set;
	fd_set				write_fds;
	fd_set	        	write_fd_set;
	size_t				i;
	int 	        	fd_dosing 	= openSerial(DOSINGUNIT_DEV, 8, 1, 'N');
	int                 fd_socket   = makeSocket(SOCKET_PORT);
	int		       		fd			= -1;
	int                 new_fd      = -1;
	int		       		maxfd		= FD_SETSIZE;
	int                 sres		= -1;
	time_t	        	cur_time;
	time_t				prev_time;
	struct timeval 		stimeout;

	daemonize();

	initSocket();
	if (listenSocket(fd_socket, 1) < 0) {
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
					dsu_read(fd_dosing, socks, &write_fd_set);
					/* Handle New Socket Connection */
				} else if (fd == fd_socket) {
					if (0 < (new_fd = acceptSocket(fd))) {
						if (0 > addSocket(new_fd)) {
							writen_ni(new_fd, "Max Sockets", strlen("Max Sockets"));
							closeSocket(new_fd);
							continue;
						}
						FD_SET(new_fd, &read_fd_set);
					}
					/* Read From Current Socket */
				} else {
					if (0 >= readSocket(fd)) {
						fprintf(stderr, "Error: reading from socket.\nClosing socket %d.\n", fd);
						freeSocket(fd);
						FD_CLR(fd, &read_fd_set);
						closeSocket(fd);
						continue;
					} 
					FD_SET(fd_dosing, &write_fd_set);
				}
			} else if (FD_ISSET(fd, &write_fds)) {
				if (fd == fd_dosing) {
					/* Write socket information to dosing unit */
					dsu_read(fd_dosing, socks, &write_fd_set);
				} else if (fd != fd_socket) {
					writeSocket(fd);
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
		freeSocket(socks[i].fd);
		closeSocket(socks[i].fd);
	}
	closeSerial(fd_dosing);
	closeSocket(fd_socket);
	exit(EXIT_SUCCESS);
}
