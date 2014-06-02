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
#ifndef __DSU_SOCKET_H__
#define __DSU_SOCKET_H__
void socketserver_set_read_event(int fd_socket, int fd_dosing);
void socketchild_set_write_event(int fd);
void socketchild_set_read_event(int fd, int fd_dosing);
bool socketchild_read_event_cb(int fd, void *data);
bool socketchild_write_event_cb(int fd, void *data);
bool socketserver_read_event_cb(int fd, void *data);
#endif //__DSU_SOCKET_H__
