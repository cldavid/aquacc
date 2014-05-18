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
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include "fd_list.h"
#include "timer.h"
#include "rrd_timer.h"

bool rrd_temperature_timer_1_cb(int __attribute__((__unused__)) fd, void __attribute__((__unused__)) *data) {
	syslog(LOG_INFO, "RRD FD_TIMER 1");
	//Destroy example FD_CLR + destroy
	//FD_CLR(fd, &read_fd_set);
	//timer_destroy(fd);
	return true;
}

void rrd_temperature_timer(void) {
	fd_list_t *fdList = aquacc_fd_list_new();

	fdList->type 	= FD_LIST_TYPE_READ_EVENT;
	fdList->istimer	= true;
	timer_init(5, &fdList->fd);
	fdList->cb = rrd_temperature_timer_1_cb;
}

