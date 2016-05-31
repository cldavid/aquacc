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
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

ssize_t writen_ni(int fd, const void *buff, size_t n) {
        size_t nl; 
        ssize_t nw; 
        const char *p; 

        p = buff;
        nl = n;
        while (nl > 0) {
                do {
                        nw = write(fd, p, nl);
                } while ( nw < 0 && errno == EINTR );
                if ( nw <= 0 ) {
			perror("Error writing to fd");
			break;
		}
                nl -= nw; 
                p += nw; 
        }   
        return n - nl; 
}

int is_valid_fd(int fd) {
        int res = fcntl(fd, F_GETFL);
        return res != -1 || errno != EBADF;
}

