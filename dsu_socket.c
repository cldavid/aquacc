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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "aquacc.h"
#include "socket.h"
#include "fd_list.h"
#include "fd_event.h"
#include "dsu_socket.h"

userdata_t *socketserver_set_userdata(socket_type_t socket_type, int srcfd) {
	userdata_t *data = calloc(1, sizeof(userdata_t));
	if (!data) {
		return NULL;
	}
	data->fd_serial = srcfd;
	data->socket_type = socket_type;
	return data;
}

void socketserver_set_read_event(int fd_socket, userdata_t *data) {
	fd_list_t *fdList = aquacc_fd_list_new();

	fdList->fd      = fd_socket;
	fdList->type    = FD_LIST_TYPE_READ_EVENT;
	fdList->istimer = false;
	fdList->data    = data;
	fdList->cb      = socketserver_read_event_cb;
}

void socketchild_set_read_event(int fd, int fd_serial) {
	fd_list_t *fdList = aquacc_fd_list_new();

	fdList->fd      = fd;
	fdList->type    = FD_LIST_TYPE_READ_EVENT;
	fdList->istimer = false;
	fdList->data    = (void *)fd_serial;
	fdList->cb      = socketchild_read_event_cb;
}

void socketchild_set_write_event(int fd) {
	fd_list_t *fdList = aquacc_fd_list_new();

	fdList->fd      = fd;
	fdList->type    = FD_LIST_TYPE_WRITE_EVENT;
	fdList->istimer = false;
	fdList->data    = NULL;
	fdList->cb      = socketchild_write_event_cb;
}

bool socketchild_read_event_cb(int fd, void *data) {
	int fd_serial = (int)data;
	syslog(LOG_INFO, "socketchild_read_event_cb OK fd %d", fd);
	if (0 >= readSocket(fd)) {
		fprintf(stderr, "Error: reading from socket.\nClosing socket %d.\n", fd);
		freeSocket(fd);
		aquacc_fd_list_delete_by_fd(fd);
		closeSocket(fd);
		return true;
	}
	syslog(LOG_INFO, "set write event fd_serial %d", fd_serial);
	set_write_event(fd_serial);
	return true;
}

bool socketchild_write_event_cb(int fd, void __attribute__((__unused__)) *data) {
	syslog(LOG_INFO, "socketchild_write_event_cb OK fd %d", fd);
	writeSocket(fd);
	clr_write_event(fd);
	return true;
}

bool socketserver_read_event_cb(int fd, void *data) {
	int 		new_fd	= -1;
	userdata_t 	*udata 	= (userdata_t *)data;
	if (!udata) {
		return false;
	}

	if (0 < (new_fd = acceptSocket(fd))) {
		if (0 > addSocket(new_fd, udata->socket_type)) {
			writen_ni(new_fd, "Max Sockets", strlen("Max Sockets"));
			closeSocket(new_fd);
			return true;
		}
		socketchild_set_read_event(new_fd, udata->fd_serial);
		socketchild_set_write_event(new_fd);
		set_read_event(new_fd);
		return true;
	}
	return true;
}

