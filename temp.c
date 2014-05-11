#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <rrd.h>

#define DIR_PATH 	"/sys/bus/w1/devices"
#define BUF_SIZE	1024
#define DATA_SIZE	4096

void logData(const char *sensorName, float temp) {

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
	DIR 			*dir		= NULL;
	struct dirent 	*dirent		= NULL;
	float			temp;

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

			logData(dirent->d_name, temp);
		}
	}
	closedir(dir);
	return 0;
}

int main(int argc, char *argv[], char *envp[]) {
	readSensors(DIR_PATH);
	return 0;
}
