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

#include <limits.h>
#include <stdbool.h>

#ifndef __AQUACC_CONFIG_H__
#define __AQUACC_CONFIG_H__

#define DELIMS   	" \n\t\r,="    			/**< tokenising delimiters         */
#define LINE_LEN 	256

typedef enum {
	NONE   = 0, 
	BOOL, 
	INT, 
	UNSIGNED, 
	FLOAT, 
	STRING, 
	HEXSTRING, 
	LINE, 
	OFILE, 
	IFILE, 
	IOFILE, 
	LFILE,
} value_t;

typedef enum {
	NO_ERR 		= 0, 
	BAD_KEYWORD 	= -1, 
	BAD_PARAMETER 	= -2,
} err_t;

typedef struct      				/**< command table structure       */
{
	char    name[NAME_MAX];    		/**< name of command               */
	value_t type;        		 	/**< type of variable              */
	void   *value;        			/**< value                         */
	void (*f)(value_t, void *, char*);  	/**< pointer to function     	   */
} script_t;

typedef struct {
	int     verbose;
	bool	foreground;
	bool    dsu_enable;
	int     dsu_socket_port;
	char	dsu_tty_port[NAME_MAX];
	int	dsu_tty_baudrate;
	bool	dsu_tty_rtscts;
	char	phmeter_tty_port[NAME_MAX];
	int	phmeter_tty_baudrate;
	bool	phmeter_tty_rtscts;
} aquacc_config_t;

int aquacc_parse_config(char *filename);
#endif //__AQUACC_CONFIG_H__
