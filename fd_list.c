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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/select.h>
#include <syslog.h>
#include "fd_list.h"
#include "fd_event.h"

static fd_list_t	*fd_list	= NULL;

bool aquacc_fd_list_type(const fd_list_t *fdlist) {
	switch(fdlist->type) {
		case FD_LIST_TYPE_UNKNOWN:
			return false;

		case FD_LIST_TYPE_TIMER:
			{
				ssize_t s;
				uint64_t exp;
				s = read(fdlist->fd, &exp, sizeof(uint64_t));
				if (s != sizeof(uint64_t)) {
					syslog(LOG_INFO, "ERROR FD_TIMER 1");
					return false;
				}
				return true;
			}
		case FD_LIST_TYPE_NORMAL:
		default:
			return true;
	}
	return false;
}

bool aquacc_fd_list_cb(int fd) {
	fd_list_t	*tmp	= fd_list;

	while(tmp) {
		if (tmp->fd == fd) {
			aquacc_fd_list_type(tmp);
			return tmp->cb(tmp->fd, tmp->data);
		}
		tmp =  tmp->next;
	}
	return false;
}

bool aquacc_fd_list_set(fd_list_t *list, int fd) {
	list->fd = fd;
	return true;
}

bool aquacc_fd_list_write_set(void) {
	fd_list_t	*tmp	= fd_list;

	while(tmp) {
		set_write_event(tmp->fd);
		tmp =  tmp->next;
	}
	return true;
}

bool aquacc_fd_list_read_set(void) {
	fd_list_t	*tmp	= fd_list;

	while(tmp) {
		set_read_event(tmp->fd);
		tmp =  tmp->next;
	}
	return true;
}

fd_list_t *aquacc_fd_list_new(void) {
	if (!fd_list) {
		if (NULL == (fd_list = calloc(1, sizeof(fd_list_t)))) {
			perror("Error allocating memory");
			return NULL;
		}
		return fd_list;
	}

	while(fd_list->next) {
		fd_list = fd_list->next;
	}

	if (NULL == (fd_list->next = calloc(1, sizeof(fd_list_t)))) {
		perror("Error allocating memory");
		return NULL;
	}
	fd_list->next->prev = fd_list;
	return fd_list->next; 
}

void aquacc_fd_list_destroy(void) {
	fd_list_t	*tmp;

	while(fd_list) {
		tmp = fd_list->next;
		free(fd_list);
		fd_list = tmp;
	}
	fd_list->next->prev = fd_list;
}
