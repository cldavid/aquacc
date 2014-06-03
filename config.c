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
#include <stddef.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <stdbool.h>
#include "config.h"

#define hexval(a) ( (((a)>='0')&&((a)<='9')) ? ((a)-'0') : ( (((a)>='A') && ((a) <='F')) ? ((a)-'A'+10) : 0 ) )
int verbose;

aquacc_config_t aquacc_config;

void _init_script (void) {
	verbose = 0;
}

const script_t script_tab[] = {
	{"verbose",          		INT,    (void *)&aquacc_config.verbose,                 NULL },
	{"dsu_enable",         		BOOL,   (void *)&aquacc_config.dsu_enable,              NULL },
	{"dsu_socket_port",         INT,    (void *)&aquacc_config.dsu_socket_port,         NULL },
};

const size_t script_tab_len	= sizeof(script_tab) / sizeof(script_t);

void hex2str (char *out, char *in) {
	while (*in) {
		*out = hexval(*in) << 4;
		in++;
		if (*in) {
			*out |= hexval(*in);
			in++;
		}
		out++;
	}
}

void str2hex (char *out, char *in) {
	const char hextab[17]="0123456789ABCDEF";
	while (*in) {
		*out++ = hextab[*in >> 4];
		*out++ = hextab[*in & 0x0F];
		in++;
	}
}

int aquacc_parse_config(char *filename)
{
	/* define local variales */
	FILE *f;          			/**< file handle            */
	char line[LINE_LEN];      	/**< input line             */
	char orig[LINE_LEN];      	/**< original input line    */
	char res[LINE_LEN];         /**< result of conversion   */
	char *p;          			/**< pointer to input line  */
	void *q;          			/**< pointer to destination */
	err_t err = NO_ERR;     	/**< script error code      */
	size_t i;            			/**< loop variable          */
	int found;            		/**< found flag             */
	int running = 1;      		/**< if true, still running */

	/* initialise */
	_init_script();

	if (NULL == (f = fopen(filename, "r"))) {
		fprintf(stderr, "Error opening %s\n", filename);
		return -1;
	}
	
	while (running && !err) {
		/* get input line */
		if (fgets (line,LINE_LEN, f) == NULL) {
			running = 0;
		}

		/* get keyword */
		strcpy(orig, line);
		p = strtok(line, DELIMS);

		/* skip empty lines, comments */
		if ((p == NULL) || (*p == '#') || (*p == ';')) {
			continue;
		}

		/* find keyword in table */
		i = 0;
		found = 0;
		for (i = 0; i < script_tab_len; i++) {
			if(!strcmp(script_tab[i].name, p)) {
				found = 1;
				break;
			}
		}

		if (found) {
			p = strtok(NULL, DELIMS);
			q = script_tab[i].value;
			if (p) {
				switch(script_tab[i].type) {
					case NONE:
						break;

					case BOOL:
						*(int*)q = ((*p != 'n') && (*p != 'N') && (*p != '0')); 
						break;

					case INT:
						*(int*)q = strtol(p, NULL, 0); 
						break;

					case UNSIGNED:
						*(unsigned int *)q = strtoul(p, NULL, 0); 
						break;

					case FLOAT: 
						*(double*)q = atof(p); 
						break;

					case STRING: 
						strcpy((char *)q, p); 
						break;

					case HEXSTRING: 
						hex2str((char *)q, p); 
						break;

					case LINE: 
						strcpy((char *)q, &orig[p - line]);
						break;

					case OFILE:
						*(FILE **)q = fopen(p, "w"); 
						break;

					case IFILE: 
						*(FILE **)q = fopen(p, "r"); 
						break;

					case IOFILE:
						*(FILE **)q = fopen(p, "rw");
						break;

					case LFILE: 
						strcpy ((char *)q, p); 
						break;

					default: 
						break;
				}
			}

			if (verbose) {
				fprintf(stdout, "scriptfile : initialised variable \"%s\" = ", script_tab[i].name);

				switch(script_tab[i].type) {
					case NONE: 
						break;

					case BOOL: 
						fprintf(stdout, "(BOOL) %s\n", *(int *)q ? "yes" : "no"); 
						break;

					case INT: 
						fprintf(stdout, "(INT) %d\n", *(int *)q);
						break;

					case UNSIGNED: 
						fprintf(stdout, "(UNSIGNED) %d\n", *(unsigned int *)q); 
						break;

					case FLOAT: 
						fprintf(stdout, "(FLOAT) %f\n", *(double *)q); 
						break;

					case STRING: 
						fprintf(stdout, "(STRING) %s\n", (char *)q); 
						break;

					case HEXSTRING: 
						str2hex(res, (char *)q); fprintf(stdout, "(HEXSTRING) %s\n", res); 
						break;

					case LINE: 
						fprintf(stdout, "(LINE) %s\n", (char *)q); 
						break;

					case OFILE: 
						fprintf(stdout, "(OUTPUT FILE) %s\n", p); 
						break;

					case IFILE: 
						fprintf(stdout, "(INPUT FILE) %s\n", p); 
						break;

					case IOFILE: 
						fprintf(stdout, "(I/O FILE) %s\n", p); 
						break;

					case LFILE: 
						fprintf(stdout, "(LOG FILE) %s\n", (char *)q); 
						break;

					default: 
						break;
				}
			}

			/* execute associated function, if any */
			p = strtok(NULL, DELIMS);
			if (script_tab[i].f) {
				script_tab[i].f(script_tab[i].type, q, p);
			}
		} else {
			err = BAD_KEYWORD;
		}

		/* error while parsing : report & exit */
		if (err) {
			switch (err) {
				case BAD_KEYWORD:
					fprintf(stderr, "unknown keyword '%s'\n", p);
					break;

				case BAD_PARAMETER:
					fprintf(stderr, "bad parameter value '%s'\n", p); 
					break;

				default:
					fprintf(stderr, "unknown error\n"); 
					break;
			}
		} 
	} 
	return err;
}


