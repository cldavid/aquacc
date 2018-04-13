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
#include <unistd.h>
#include <stdint.h>
#include "aquacc.h"
#include "serial.h"
#include "dsu_socket.h"
#include "socket.h"
#include "daemon.h"
#include "dsu.h"
#include "phmeter.h"
#include "timer.h"
#include "fd_list.h"
#include "rrd_timer.h"
#include "fd_event.h"
#include "config.h"

unsigned 	int alive			= 1;
extern 		int errno;
extern 		aquacc_config_t aquacc_config;

void aquacc_log(const char *msg) {
	syslog(LOG_INFO, "%s", msg);
	return;
}

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)), char *envp[] __attribute__ ((unused))) {
	fd_set              read_fds;
	fd_set				write_fds;
	int		       		fd			= -1;
	int		       		maxfd		= FD_SETSIZE;
	int                 sres		= -1;
	time_t	        	cur_time;
	struct timeval 		stimeout;

	if (0 > aquacc_parse_config("/etc/aquacc.conf")) {
		exit(EXIT_FAILURE);
	}

	printf("Verbose level %d\n", aquacc_config.verbose);
	daemonize();
	openlog("aquacc", LOG_PID, LOG_USER);

	/* Start DSU components */
	initSocket();
	dsu_init();
	phmeter_init();

	/* RRD Timer */
	rrd_set_temperature_timer();


	time(&cur_time);

	zero_read_event();
	zero_write_event();
	aquacc_fd_list_read_set();
	aquacc_fd_list_write_set();
	while (alive) {
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
				if (aquacc_fd_list_read_cb(fd)) {
					syslog(LOG_INFO, "aquacc_fd_list_read_cb OK fd %d", fd);
				} 
			} else if (FD_ISSET(fd, &write_fds)) {
				syslog(LOG_INFO, "write_fds OK fd %d", fd);
				if (aquacc_fd_list_write_cb(fd)) {
					syslog(LOG_INFO, "aquacc_fd_list_write_cb OK fd %d", fd);
				} 
			}
		}
	}

exit:
	/* Cleanup */
	dsu_exit();
	phmeter_exit();
	aquacc_fd_list_destroy();
	closelog();
	exit(EXIT_SUCCESS);
}
