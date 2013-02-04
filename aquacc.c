#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

ssize_t writen_ni(int fd, const void *buff, size_t n) {
        size_t nl; 
        ssize_t nw; 
        const char *p; 

        p = buff;
        nl = n;
        while (nl > 0) {
                do {
                        nw = write(fd, p, nl);
                } while ( nw < 0 && errno == EINTR );
                if ( nw <= 0 ) break;
                nl -= nw; 
                p += nw; 
        }   
        return n - nl; 
}

int is_valid_fd(int fd) {
        int res = fcntl(fd, F_GETFL);
        return res != -1 || errno != EBADF;
}

