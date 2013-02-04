#ifndef __SERIAL_H__
#define __SERIAL_H__

#define DOSINGUNIT_DEV          "/dev/ttyACM0"
//#define DOSINGUNIT_DEV                "/dev/sahUSB0"

int openSerial(const char *port,unsigned int bits,unsigned int stop,char parity);
int closeSerial(int fd);
ssize_t readSerial(int fd,char *buffer,ssize_t size);
#endif //__SERIAL_H_
