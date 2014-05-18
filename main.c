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
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include "aquacc.h"
#include "serial.h"
#include "socket.h"
#include "daemon.h"
#include "dsu.h"
#include "timer.h"
#include "fd_list.h"
#include "rrd_timer.h"
#include "fd_event.h"

extern aq_socket_t	socks[MAX_SOCKETS];

unsigned 	int alive			= 1;
extern 		int errno;

void aquacc_log(const char *msg) {
	syslog(LOG_INFO, msg);
	return;
}

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)), char *envp[] __attribute__ ((unused))) {
	fd_set              read_fds;
	fd_set				write_fds;
	size_t				i;
	int 	        	fd_dosing 	= openSerial(DOSINGUNIT_DEV, 8, 1, 'N');
	int                 fd_socket   = makeSocket(SOCKET_PORT);
	int		       		fd			= -1;
	int                 new_fd      = -1;
	int		       		maxfd		= FD_SETSIZE;
	int                 sres		= -1;
	time_t	        	cur_time;
	struct timeval 		stimeout;

	daemonize();

	initSocket();
	if (listenSocket(fd_socket, 1) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* DSU */
	dsu_set_read_event(fd_dosing, socks);
	dsu_set_write_event(fd_dosing, socks);

	/* RRD Timer */
	rrd_set_temperature_timer();

	/* setUnixTime Timer */
	dsu_set_setUnixTime_timer(fd_dosing);

	openlog("aquacc", LOG_PID, LOG_USER);

	time(&cur_time);

	zero_read_event();
	zero_write_event();
	set_read_event(fd_socket);
	aquacc_fd_list_read_set();
	while (alive && is_valid_fd(fd_dosing) && is_valid_fd(fd_socket)) {
		read_fds 	= get_read_event();
		write_fds	= get_write_event();

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
				/* Handle New Socket Connection */
				if (fd == fd_socket) {
					aquacc_log("read fd_socket TST");
					if (0 < (new_fd = acceptSocket(fd))) {
						if (0 > addSocket(new_fd)) {
							writen_ni(new_fd, "Max Sockets", strlen("Max Sockets"));
							closeSocket(new_fd);
							continue;
						}
						set_read_event(new_fd);
					}
				} else if (aquacc_fd_list_read_cb(fd)) {
					syslog(LOG_INFO, "aquacc_fd_list_read_cb OK fd %d", fd);
				} else {
					aquacc_log("read else TST");
					if (0 >= readSocket(fd)) {
						fprintf(stderr, "Error: reading from socket.\nClosing socket %d.\n", fd);
						freeSocket(fd);
						clr_read_event(fd);
						closeSocket(fd);
						continue;
					} 
					set_write_event(fd_dosing);
				}
			} else if (FD_ISSET(fd, &write_fds)) {
				if (aquacc_fd_list_write_cb(fd)) {
					syslog(LOG_INFO, "aquacc_fd_list_write_cb OK fd %d", fd);
				} else if (fd != fd_socket) {
					aquacc_log("write !fd_socket TST");
					writeSocket(fd);
					clr_write_event(fd);
				} 
			}
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
	aquacc_fd_list_destroy();
	closelog();
	exit(EXIT_SUCCESS);
}
