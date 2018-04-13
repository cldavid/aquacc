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
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include "aquacc.h"
#include "serial.h"
#include "fd_list.h"
#include "fd_event.h"
#include "timer.h"
#include "dsu.h"
#include "dsu_socket.h"
#include "socket.h"
#include "config.h"

extern aq_socket_t  socks[MAX_SOCKETS];
extern aquacc_config_t aquacc_config;

static int fd_dosing;
static int fd_socket;

void dsu_init(void) {
	char 		*serial_port 	= aquacc_config.dsu_tty_port;
	int 		serial_baudrate = aquacc_config.dsu_tty_baudrate;
	bool 		serial_rtscts  	= aquacc_config.dsu_tty_rtscts;
	userdata_t 	*data 		= NULL;

	fd_dosing   = openSerial(serial_port, 8, 1, 'N', serial_baudrate, serial_rtscts);
	fd_socket   = makeSocket(aquacc_config.dsu_socket_port);

	if (fd_dosing <= 0) {
		printd("Error opening serial port.\n");
		exit(EXIT_FAILURE);
	}

	if (listenSocket(fd_socket, 1) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* DSU Socket Server */
	data = socketserver_set_userdata(SOCKET_TYPE_DSU, fd_dosing); 

	socketserver_set_read_event(fd_socket, data);

	/* DSU */
	dsu_set_read_event(fd_dosing, socks);
	dsu_set_write_event(fd_dosing, socks);

	/* DSU setUnixTime Timer */
	dsu_set_unixtime_timer(fd_dosing);
}

void dsu_exit(void) {
	size_t i;
	for (i = 0; i < MAX_SOCKETS; i++) {
		freeSocket(socks[i].fd);
		closeSocket(socks[i].fd);
	}
	closeSerial(fd_dosing);
	closeSocket(fd_socket);
}

void dsu_set_write_event(int fd_dosing, aq_socket_t *socks) {
	fd_list_t *fdList = aquacc_fd_list_new();

	fdList->fd	= fd_dosing;
	fdList->type 	= FD_LIST_TYPE_WRITE_EVENT;
	fdList->istimer	= false;
	fdList->data    = socks;
	fdList->cb      = dsu_write_event_cb;
}

void dsu_set_read_event(int fd_dosing, aq_socket_t *socks) {
	fd_list_t *fdList = aquacc_fd_list_new();

	fdList->fd	= fd_dosing;
	fdList->type 	= FD_LIST_TYPE_READ_EVENT;
	fdList->istimer	= false;
	fdList->data    = socks;
	fdList->cb      = dsu_read_event_cb;
}

void dsu_set_unixtime_timer(int fd_dosing) {
	fd_list_t *fdList = aquacc_fd_list_new();

	timer_init(SET_TIME_INTERVAL, &fdList->fd);
	fdList->type 	= FD_LIST_TYPE_READ_EVENT;
	fdList->istimer	= true;
	fdList->data    = (void *)fd_dosing;
	fdList->cb      = dsu_timer_setUnixTime_cb;
}

bool dsu_read_event_cb(int fd, void *data) {
	aq_socket_t *socks = data;

	syslog(LOG_INFO, "dsu_read_event_cb %d", fd);
	dsu_read(fd, socks);
	return true;
}

bool dsu_write_event_cb(int fd, void *data) {
	aq_socket_t *socks = data;
	syslog(LOG_INFO, "dsu_write_event_cb %d", fd);
	dsu_write(fd, socks);
	return true;
}

ssize_t dsu_setUnixTime(int fd, time_t cur_time) {
	char    string[250];
	int     len;
	len = snprintf(string, sizeof(string), "setUnixTime %lu\r\n", cur_time);
	return(writen_ni(fd, string, len));
}

/* Write current the current time to the dosing unit */
bool dsu_timer_setUnixTime_cb(int __attribute__ ((unused)) fd, void *data) {
	time_t cur_time;
	int fd_dosing = (int)data;

	time(&cur_time);
	dsu_setUnixTime(fd_dosing, cur_time);
	return true;
}

void dsu_write(int fd, aq_socket_t socks[]) {
	size_t i;

	/* Write socket information to dosing unit */
	for (i = 0; i < MAX_SOCKETS; i++) {
		if (socks[i].free && socks[i].has_r_data && (socks[i].socket_type == SOCKET_TYPE_DSU)) {
			writen_ni(fd, socks[i].r_buffer, socks[i].r_len);
			socks[i].has_r_data = 0;
			socks[i].r_len = 0;
		}
	}
	clr_write_event(fd);
}

void dsu_read(int fd, aq_socket_t socks[]) {
	char                dos_buffer[MAXMSG];
	size_t				i;
	ssize_t             dos_len;

	dos_len = readSerial(fd, dos_buffer, sizeof(dos_buffer));
	if (dos_len >= 0) {
		/* Copy Buffer To All Connected Sockets */
		for (i = 0; i < MAX_SOCKETS; i++) {
			if (socks[i].free && (socks[i].socket_type == SOCKET_TYPE_DSU)) {
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
				set_write_event(socks[i].fd);
			}
		}
	}
}
