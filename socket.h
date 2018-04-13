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
#ifndef __SOCKET_H__
#define __SOCKET_H__

int readSocket(int fd);
int writeSocket(int fd);
int freeSocket(int fd);
int addSocket(int fd, socket_type_t socket);
void initSocket(void);
int acceptSocket(int sock);
int makeSocket(int16_t port);
int listenSocket(int sockfd, int backlog);
int closeSocket(int fd);

#endif //__SOCKET_H__
