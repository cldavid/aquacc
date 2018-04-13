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

#ifndef __AQUACC_PHMETER_H__
#define __AQUACC_PHMETER_H__
void phmeter_init(void);
void phmeter_exit(void);
void phmeter_set_read_event(int fd_dosing, aq_socket_t *socks);
void phmeter_set_write_event(int fd_dosing, aq_socket_t *socks);
bool phmeter_read_event_cb(int fd, void *data);
bool phmeter_write_event_cb(int fd, void *data);
void phmeter_set_phresult_timer(int fd_dosing);
ssize_t phmeter_getPH(int fd);
bool phmeter_timer_phresult_cb(int fd, void *data);
void phmeter_write(int fd, aq_socket_t socks[]);
void phmeter_read(int fd, aq_socket_t socks[]);
#endif //__AQUACC_PHMETER_H__
