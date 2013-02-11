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
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "aquacc.h"

extern 		int errno;

ssize_t readSerial(int fd, char *buffer, ssize_t size) {
	ssize_t	nbytes	= 0;

	do {
		nbytes = read(fd, buffer, size-1);
	} while (nbytes < 0 && errno == EINTR);
	buffer[nbytes] = '\0';
	printf("%s", buffer);
	return nbytes;
}

int closeSerial(int fd) {
    return(close(fd));
}

int openSerial(const char *port, unsigned int bits __attribute__ ((unused)), unsigned int stop __attribute__ ((unused)), char parity __attribute__ ((unused))) {
	struct termios 	tty;

	int fd 	= open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if (0 > fd) {
		printd("Error opening serial port %s\n", port);
                return fd;
	}

	/*
	 * Get the current tty for the port...
	 */
	memset(&tty, 0, sizeof(tty));
	tcgetattr(fd, &tty);
	cfmakeraw(&tty);

	/*
	 * Set the baud rates to 19200...
	 */
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	/*
	 * Enable the receiver and set local mode...
	 */
	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag |= CLOCAL | CREAD;
	tty.c_cflag &= ~HUPCL;          

	tty.c_cflag |= CRTSCTS;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); 

	tty.c_cc[VMIN]  = 1;
	tty.c_cc[VTIME] = 5;
	/*
	 * Set the new tty for the port...
	 */
	tcsetattr(fd, TCSANOW, &tty);
	tcflush(fd, TCIOFLUSH);
	return fd;
}

