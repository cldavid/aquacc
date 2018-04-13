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
#include "phmeter.h"
#include "dsu_socket.h"
#include "socket.h"
#include "config.h"

extern aq_socket_t  socks[MAX_SOCKETS];
extern aquacc_config_t aquacc_config;

static int fd_phmeter;
static int fd_socket;

void phmeter_init(void) {
	char 		*serial_port 	= aquacc_config.phmeter_tty_port;
	int 		serial_baudrate = aquacc_config.phmeter_tty_baudrate;
	bool 		serial_rtscts  	= aquacc_config.phmeter_tty_rtscts;
        userdata_t      *data           = NULL;
	
	fprintf(stdout, "openSerial %s %d %d\n", serial_port, serial_baudrate, serial_rtscts);
	fd_phmeter  = openSerial(serial_port, 8, 1, 'N', serial_baudrate, serial_rtscts);
	fd_socket   = makeSocket(aquacc_config.phmeter_socket_port);

	fprintf(stdout, "PH-METER FD %d SOCKET %d\n", fd_phmeter, fd_socket);
	if (fd_phmeter <= 0) {
		printd("Error opening serial port.\n");
		exit(EXIT_FAILURE);
	}

	if (listenSocket(fd_socket, 1) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* PH-METER Socket Server */
        data = socketserver_set_userdata(SOCKET_TYPE_PHMETER, fd_phmeter);
	socketserver_set_read_event(fd_socket, data);

	/* PH-METER */
	phmeter_set_read_event(fd_phmeter, socks);
	phmeter_set_write_event(fd_phmeter, socks);

	/* PH-METER RESULT Timer */
	phmeter_set_phresult_timer(fd_phmeter);
}

void phmeter_exit(void) {
	size_t i;
	for (i = 0; i < MAX_SOCKETS; i++) {
		freeSocket(socks[i].fd);
		closeSocket(socks[i].fd);
	}
	closeSerial(fd_phmeter);
	closeSocket(fd_socket);
}

void phmeter_set_write_event(int fd_phmeter, aq_socket_t *socks) {
	fd_list_t *fdList = aquacc_fd_list_new();

	fdList->fd	= fd_phmeter;
	fdList->type 	= FD_LIST_TYPE_WRITE_EVENT;
	fdList->istimer	= false;
	fdList->data    = socks;
	fdList->cb      = phmeter_write_event_cb;
}

void phmeter_set_read_event(int fd_phmeter, aq_socket_t *socks) {
	fd_list_t *fdList = aquacc_fd_list_new();

	fdList->fd	= fd_phmeter;
	fdList->type 	= FD_LIST_TYPE_READ_EVENT;
	fdList->istimer	= false;
	fdList->data    = socks;
	fdList->cb      = phmeter_read_event_cb;
}

void phmeter_set_phresult_timer(int fd_phmeter) {
	fd_list_t *fdList = aquacc_fd_list_new();

	timer_init(SET_TIME_INTERVAL, &fdList->fd);
	fdList->type 	= FD_LIST_TYPE_READ_EVENT;
	fdList->istimer	= true;
	fdList->data    = (void *)fd_phmeter;
	fdList->cb      = phmeter_timer_phresult_cb;
}

bool phmeter_read_event_cb(int fd, void *data) {
	aq_socket_t *socks = data;

	syslog(LOG_INFO, "phmeter_read_event_cb %d", fd);
	phmeter_read(fd, socks);
	return true;
}

bool phmeter_write_event_cb(int fd, void *data) {
	aq_socket_t *socks = data;
	syslog(LOG_INFO, "phmeter_write_event_cb %d", fd);
	phmeter_write(fd, socks);
	return true;
}

ssize_t phmeter_getPHresult(int fd) {
	fprintf(stdout, "W PH-METER FD %d - (R)\n", fd);
	return(writen_ni(fd, "r\r", 2));
}

/* Write current the current time to the phmeter unit */
bool phmeter_timer_phresult_cb(int __attribute__ ((unused)) fd, void *data) {
	int fd_phmeter = (int)data;

	phmeter_getPHresult(fd_phmeter);
	return true;
}

void phmeter_write(int fd, aq_socket_t socks[]) {
	size_t i;

	/* Write socket information to phmeter unit */
	for (i = 0; i < MAX_SOCKETS; i++) {
		if (socks[i].free && socks[i].has_r_data && (socks[i].socket_type == SOCKET_TYPE_PHMETER)) {
			replaceChar(socks[i].r_buffer, '\n', '\r');
			writen_ni(fd, socks[i].r_buffer, socks[i].r_len);
			socks[i].has_r_data = 0;
			socks[i].r_len = 0;
		}
	}
	clr_write_event(fd);
}

void phmeter_read(int fd, aq_socket_t socks[]) {
	char                phmeter_buffer[MAXMSG];
	size_t	            i;
	ssize_t             phmeter_len;
	
	fprintf(stdout, "PH-METER READ FD %d\n", fd);
	phmeter_len = readSerial(fd, phmeter_buffer, sizeof(phmeter_buffer));
	replaceChar(phmeter_buffer, '\r', '\n');

	fprintf(stdout, "PH-METER READ FD %d LEN %d\n", fd, phmeter_len);
	if (phmeter_len >= 0) {
		/* Copy Buffer To All Connected Sockets */
		for (i = 0; i < MAX_SOCKETS; i++) {
			if (socks[i].free && (socks[i].socket_type == SOCKET_TYPE_PHMETER)) {
				if (socks[i].has_w_data) {
					printf("PH-METER: Appending wlen %u + phmeter_len %u\n", socks[i].w_len, phmeter_len);
					memcpy(socks[i].w_buffer + socks[i].w_len, phmeter_buffer, phmeter_len);
					socks[i].has_w_data = 1;
					socks[i].w_len += phmeter_len;
				} else {
					memcpy(socks[i].w_buffer, phmeter_buffer, phmeter_len);
					socks[i].has_w_data = 1;
					socks[i].w_len = phmeter_len;
				}
				set_write_event(socks[i].fd);
			}
		}
	}
}
