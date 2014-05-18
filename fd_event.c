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
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static fd_set _read_fd_set;
static fd_set _write_fd_set;

void clr_read_event(int fd) {
	FD_CLR(fd, &_read_fd_set);
}

void clr_write_event(int fd) {
	FD_CLR(fd, &_write_fd_set);
}
void zero_read_event(void) {
	FD_ZERO(&_read_fd_set);
}

void zero_write_event(void) {
	FD_ZERO(&_write_fd_set);
}

fd_set get_read_event(void) {
	return _read_fd_set;
}

fd_set get_write_event(void) {
	return _write_fd_set;
}

void set_read_event(int fd) {
	FD_SET(fd, &_read_fd_set);
}

void set_write_event(int fd) {
	FD_SET(fd, &_write_fd_set);
}
