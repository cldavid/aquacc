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
#ifndef __AQUACC_H__
#define __AQUACC_H__

#define SET_TIME_INTERVAL               60
#define printd                          printf
#define MAXMSG                          8192
#define MAX_SOCKETS                     10

typedef enum {
        SOCKET_TYPE_UNSET   = 0,
	SOCKET_TYPE_DSU,
	SOCKET_TYPE_PHMETER
} socket_type_t;

typedef struct {
        unsigned int    id; 
	socket_type_t	socket_type;
        unsigned int    free;
        int                             fd; 
        unsigned int    has_r_data;
        unsigned int    has_w_data;
        char                    r_buffer[MAXMSG];
        char                    w_buffer[MAXMSG];
        size_t                  r_len;
        size_t                  w_len;
} aq_socket_t;

ssize_t writen_ni(int fd, const void *buff, size_t n);
int is_valid_fd(int fd);
char *replaceChar(char *str, char oldChar, char newChar);
#endif //__AQUACC_H__
