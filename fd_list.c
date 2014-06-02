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

bool aquacc_fd_list_type_cb(const fd_list_t *fdlist) {
	if (fdlist->istimer) {
		ssize_t s;
		uint64_t exp;
		s = read(fdlist->fd, &exp, sizeof(uint64_t));
		if (s != sizeof(uint64_t)) {
			syslog(LOG_INFO, "ERROR FD_TIMER 1");
			return false;

		}
	}

	switch(fdlist->type) {
		case FD_LIST_TYPE_READ_EVENT:
			return fdlist->cb(fdlist->fd, fdlist->data);

		case FD_LIST_TYPE_WRITE_EVENT:
			return fdlist->cb(fdlist->fd, fdlist->data);
		default:
			return false;
	}
}

fd_list_t *aquacc_fd_list_find(int fd, enum fd_list_type type) {
	fd_list_t	*tmp	= fd_list;

	while(tmp) {
		if ((tmp->fd == fd) && (tmp->type == type)) {
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

bool aquacc_fd_list_read_cb(int fd) {
	fd_list_t	*tmp	= aquacc_fd_list_find(fd, FD_LIST_TYPE_READ_EVENT);

	if (tmp) {
		return(aquacc_fd_list_type_cb(tmp));
	}
	return false;
}

bool aquacc_fd_list_write_cb(int fd) {
	fd_list_t	*tmp	= aquacc_fd_list_find(fd, FD_LIST_TYPE_WRITE_EVENT);

	if (tmp) {
		return(aquacc_fd_list_type_cb(tmp));
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

void aquacc_fd_list_delete_by_fd(int fd) {
	aquacc_read_fd_list_delete_by_fd(fd);
	aquacc_write_fd_list_delete_by_fd(fd);
}

void aquacc_read_fd_list_delete_by_fd(int fd) {
	fd_list_t *list = aquacc_fd_list_find(fd, FD_LIST_TYPE_READ_EVENT);
	aquacc_fd_list_delete(list);
}

void aquacc_write_fd_list_delete_by_fd(int fd) {
	fd_list_t *list = aquacc_fd_list_find(fd, FD_LIST_TYPE_WRITE_EVENT);
	aquacc_fd_list_delete(list);
}

void aquacc_fd_list_delete(fd_list_t *fdlist) {
	if (fdlist) {
		if (fdlist->type == FD_LIST_TYPE_READ_EVENT) {
			clr_read_event(fdlist->fd);
		} else if (fdlist->type == FD_LIST_TYPE_WRITE_EVENT) {
			clr_write_event(fdlist->fd);
		}

		//Detach fd_list from linked list;
		if (fdlist->prev) {
			fdlist->prev->next = fdlist->next;
		}

		if (fdlist->next) {
			fdlist->next->prev = fdlist->prev;
		}
		free(fdlist);
	}
}

fd_list_t *aquacc_fd_list_new(void) {
	fd_list_t	*tmp	= fd_list;

	if (!fd_list) {
		if (NULL == (fd_list = calloc(1, sizeof(fd_list_t)))) {
			perror("Error allocating memory");
			return NULL;
		}
		return fd_list;
	}

	while(tmp->next) {
		tmp = tmp->next;
	}

	if (NULL == (tmp->next = calloc(1, sizeof(fd_list_t)))) {
		perror("Error allocating memory");
		return NULL;
	}
	tmp->next->prev = tmp;
	return tmp->next; 
}

void aquacc_fd_list_destroy(void) {
	fd_list_t	*tmp;

	while(fd_list) {
		tmp = fd_list->next;
		free(fd_list);
		fd_list = tmp;
	}
}
