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

#ifndef __FD_LIST_T_H__
#define __FD_LIST_T_H__
enum fd_list_type {
	FD_LIST_TYPE_UNKNOWN,
	FD_LIST_TYPE_TIMER,
	FD_LIST_TYPE_READ_EVENT,
	FD_LIST_TYPE_WRITE_EVENT,
};

struct _fd_list_t {
	int 				fd;
	enum fd_list_type	type;
	void				*data;
	bool				istimer;
	bool				(*cb)(int fd, void *data);
	struct _fd_list_t	*next;
	struct _fd_list_t	*prev;
};
typedef struct _fd_list_t	fd_list_t;

fd_list_t *aquacc_fd_list_find(int fd, enum fd_list_type type);
void aquacc_fd_list_delete_by_fd(int fd);
void aquacc_read_fd_list_delete_by_fd(int fd);
void aquacc_write_fd_list_delete_by_fd(int fd);
bool aquacc_fd_list_type_cb(const fd_list_t *fdlist);
bool aquacc_fd_list_read_cb(int fd);
bool aquacc_fd_list_write_cb(int fd);
bool aquacc_fd_list_write_set(void);
bool aquacc_fd_list_read_set(void);
bool aquacc_fd_list_set(fd_list_t *list, int fd);
fd_list_t *aquacc_fd_list_new(void);
void aquacc_fd_list_delete(fd_list_t *fdlist);
void aquacc_fd_list_destroy(void);

#endif //__FD_LIST_T_H__
