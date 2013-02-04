#ifndef __AQUACC_H__
#define __AQUACC_H__

#define SET_TIME_INTERVAL               60
#define printd                          printf
#define SOCKET_PORT                     5000 
#define MAXMSG                          8192
#define MAX_SOCKETS                     10

typedef struct {
        unsigned int    id; 
        unsigned int    free;
        int                             fd; 
        unsigned int    has_r_data;
        unsigned int    has_w_data;
        char                    r_buffer[MAXMSG];
        char                    w_buffer[MAXMSG];
        size_t                  r_len;
        size_t                  w_len;
} aq_socket_t;

ssize_t writen_ni(int fd, const void *buff, size_t n);
int is_valid_fd(int fd);
#endif //__AQUACC_H__
