#ifndef __SOCKET_H__
#define __SOCKET_H__

int readSocket(int fd);
int writeSocket(int fd);
int freeSocket(int fd);
int addSocket(int fd);
void initSocket(void);
int acceptSocket(int sock);
int makeSocket(int16_t port);
int listenSocket(int sockfd, int backlog);
int closeSocket(int fd);

#endif //__SOCKET_H__
