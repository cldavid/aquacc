#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <rrd.h>
#include <math.h>

#define RRDTOOL			"/usr/bin/rrdtool"
#define RRD_DB_PATH		"/home/pi/aquacc/multirPItemp.rrd"
#define DIR_PATH 		"/sys/bus/w1/devices"
#define BUF_SIZE		1024
#define DATA_SIZE		4096
#define NUM_SENSOR_MAX	10

typedef struct {
	bool			active;
	char			sensorName[256];
	float			temp;
} sensor_t;

void logData(sensor_t sdata[], size_t arr_len) {
	char system_cmd[256];
	size_t i	= 0;
	size_t len	= 0;

	len = snprintf(system_cmd + len, sizeof(system_cmd) - len, RRDTOOL" update "RRD_DB_PATH" N");
	for (i = 0; i < arr_len; i++) {
		if (sdata[i].active) {
			len += snprintf(system_cmd + len, sizeof(system_cmd) - len, ":%.0f", roundf(sdata[i].temp) );
		}
	}
	printf("%s\n", system_cmd);
	system(system_cmd);
}

int readSensor(const char *sensorName, const char *file, float *temp) {
	char 	buf[BUF_SIZE];
	char 	data[DATA_SIZE];
	char	*tString	= NULL;
	ssize_t numRead 	= 0;
	ssize_t	len			= 0;
	int 	fd 			= -1;
	float	t			= 0.0;

	if (0 > (fd = open(file, O_RDONLY))) {
		perror("Couldn't open w1 device.");
		return -1;   
	}

	while((numRead = read(fd, buf, sizeof(buf))) > 0) {
		len += snprintf(data + len, sizeof(data) - len, "%s", buf);
	}
	close(fd);
	tString = strstr(buf, "t=") + 2;
	t	 	= strtof(tString, NULL) / 1000;

	if (t == 85.000) {
		fprintf(stderr, "Sensor: %s - Read invalid temperature %f\n", sensorName, t);
		return -1;
	}

	printf("Sensor: %s  - Temp: %.3f\n", sensorName, t); 
	*temp = t;
	return 0;
}

int readSensors(const char *path, sensor_t sdata[], size_t len) {
	char 			fileName[PATH_MAX + NAME_MAX];
	DIR 			*dir		= NULL;
	struct dirent 	*dirent		= NULL;
	float			temp		= 0.0;
	size_t			i 			= 0;

	dir = opendir(path);
	if (!dir) {
		perror ("Couldn't open the w1 devices directory");
		return -1;
	}

	while ((dirent = readdir(dir))) {
		// 1-wire devices are links beginning with 28-
		if (dirent->d_type == DT_LNK && strstr(dirent->d_name, "28-") != NULL) {
			snprintf(fileName, sizeof(fileName), "%s/%s/w1_slave", path, dirent->d_name);
			if (0 > readSensor(dirent->d_name, fileName, &temp)) {
				continue;
			}

			if (len > i) {
				sdata[i].active = true;
				strncpy(sdata[i].sensorName, dirent->d_name, sizeof(sdata[i].sensorName) - 1);
				sdata[i].temp = temp;
				i++;
			} else {
				fprintf(stderr, "Error too much sensors detected\n");
				break;
			}
		}
	}
	closedir(dir);
	return 0;
}

int main(int __attribute__((__unused__)) argc, char __attribute__((__unused__)) *argv[], char __attribute__((__unused__)) *envp[]) {
	sensor_t		sdata[NUM_SENSOR_MAX];
	size_t			len = NUM_SENSOR_MAX;

	memset(sdata, 0, sizeof(sdata));

	readSensors(DIR_PATH, sdata, len);
	logData(sdata, len);
	return 0;
}
