#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "daemon.h"

int daemonize(void) {
        pid_t   pid, sid;

        pid = fork();
        if (pid < 0) {
                exit(EXIT_FAILURE);
        } else if (pid > 0) {
                exit(EXIT_SUCCESS);
        }   

        umask(0);
        sid = setsid();
        if (sid < 0) {
                exit(EXIT_FAILURE);
        }   

        if ((chdir("/")) < 0) {
                exit(EXIT_FAILURE);
        }   

        syslog(LOG_NOTICE, "aquacc started by User %d", getuid ());
        return 0;
}

