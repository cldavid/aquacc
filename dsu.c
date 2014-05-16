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
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include "aquacc.h"
#include "serial.h"

void dsu_write(int fd, aq_socket_t socks[], fd_set *write_fd_set) {
	size_t i;

	/* Write socket information to dosing unit */
	for (i = 0; i < MAX_SOCKETS; i++) {
		if (socks[i].free && socks[i].has_r_data) {
			writen_ni(fd, socks[i].r_buffer, socks[i].r_len);
			socks[i].has_r_data = 0;
			socks[i].r_len = 0;
		}
	}
	FD_CLR(fd, write_fd_set);
}

void dsu_read(int fd, aq_socket_t socks[], fd_set *write_fd_set) {
	char                dos_buffer[MAXMSG];
	size_t				i;
	ssize_t             dos_len;

	dos_len = readSerial(fd, dos_buffer, sizeof(dos_buffer));
	if (dos_len >= 0) {
		/* Copy Buffer To All Connected Sockets */
		for (i = 0; i < MAX_SOCKETS; i++) {
			if (socks[i].free) {
				if (socks[i].has_w_data) {
					printf("Appending wlen %u + dos_len %u\n", socks[i].w_len, dos_len);
					memcpy(socks[i].w_buffer + socks[i].w_len, dos_buffer, dos_len);
					socks[i].has_w_data = 1;
					socks[i].w_len += dos_len;
				} else {
					memcpy(socks[i].w_buffer, dos_buffer, dos_len);
					socks[i].has_w_data = 1;
					socks[i].w_len = dos_len;
				}
				FD_SET(socks[i].fd, write_fd_set);
			}
		}
	}
}
