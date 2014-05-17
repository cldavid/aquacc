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
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h> 
#include <string.h>
#include <stdbool.h>

bool timer_set(int fd, unsigned int sec) {
	struct itimerspec 	timerSpec;

	memset(&timerSpec, 0, sizeof(timerSpec));

	timerSpec.it_value.tv_sec 	= sec;
	timerSpec.it_value.tv_nsec 	= 0;
	timerSpec.it_interval.tv_sec = sec;
	timerSpec.it_interval.tv_nsec = 0;

	if (0 > timerfd_settime(fd, TFD_TIMER_ABSTIME, &timerSpec, NULL)) {
		perror("Error destroying timer");
		return false;
	}
	return true;
}

bool timer_init(unsigned int sec, int *fd_out) {
	struct itimerspec 	timerSpec;
	int 				fd;

	memset(&timerSpec, 0, sizeof(timerSpec));
	if (0 > (fd = timerfd_create(CLOCK_REALTIME, 0))) {
		perror("Error creating timer");
		return false;
	}

	if (!timer_set(fd, sec)) {
		return false;
	}
	*fd_out = fd;
	return true;
}

bool timer_destroy(int fd) {
	timer_set(fd, 0);
	close(fd);
	return true;
}

