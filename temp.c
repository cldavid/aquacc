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

#define SENSOR_1_NAME	"28-0000053c55c1"
#define	SENSOR_2_NAME	"28-0000053c6b1f"

typedef struct {
	bool			active;
	char			sensorName[32];
	float			temp;
} sensor_t;

sensor_t sensors[] = {
	{	true,	SENSOR_1_NAME, 0.0	},
	{	true,	SENSOR_2_NAME, 0.0	},
};

void logData() {
	char system_cmd[256];
	size_t i	= 0;
	size_t len	= 0;

	len = snprintf(system_cmd + len, sizeof(system_cmd) - len, RRDTOOL" update "RRD_DB_PATH" N");
	for (i = 0; i < sizeof(sensors) / sizeof(sensor_t); i++) {
		if (sensors[i].active) {
			len += snprintf(system_cmd + len, sizeof(system_cmd) - len, ":%.1f", sensors[i].temp);
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

int readSensors(const char *path) {
	char 			fileName[PATH_MAX + NAME_MAX];
	size_t			i 			= 0;

	for (i = 0; i < sizeof(sensors) / sizeof(sensor_t); i++) {
		snprintf(fileName, sizeof(fileName), "%s/%s/w1_slave", path, sensors[i].sensorName);
		if (0 > readSensor(sensors[i].sensorName, fileName, &sensors[i].temp)) {
			continue;
		}
	}
	return 0;
}

int main(int __attribute__((__unused__)) argc, char __attribute__((__unused__)) *argv[], char __attribute__((__unused__)) *envp[]) {
	readSensors(DIR_PATH);
	logData();
	return 0;
}
