CC	= gcc
CC	= /home/sah5056/x-tools/mips-unknown-linux-uclibc/bin/mips-unknown-linux-uclibc-gcc
#CC	= /build/arduino/openwrt-yun/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-gcc
CFLAGS += -g -Wall -Wextra -lc -lm -static
CFLAGS += -g -Wall -Wextra -lc -lm 
LDFLAGS += 

TARGET	= aquacc
SRCS	= main.c aquacc.c serial.c socket.c dsu_socket.c daemon.c dsu.c timer.c fd_list.c rrd_timer.c fd_event.c config.c
OBJS	= $(SRCS:.c=.o)

TEMP_TARGET	= temp
TEMP_SRCS	= temp.c
TEMP_OBJS	= $(TEMP_SRCS:.c=.o)

SERIALT_TARGET	= test_serial
SERIALT_SRCS	= serial.c config.c test_serial.c
SERIALT_OBJS	= $(SERIALT_SRCS:.c=.o)

.PHONY: all clean install 

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(SRCS) $(TARGET) $(TEMP_TARGET) 

$(SERIALT_TARGET): $(SERIALT_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SERIALT_OBJS) $(LDFLAGS)

$(TEMP_TARGET): $(TEMP_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TEMP_OBJS) $(LDFLAGS)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:  
	rm -rf $(TARGET) $(TEMP_TARGET) $(OBJS) $(TEMP_OBJS)

install:
	@echo "INSTALL" 
	install -d /usr/local/bin
	install -m 0755 aquacc /usr/local/bin/aquacc
	install -m 0755 etc/init.d/aquacc /etc/init.d/aquacc
	@echo
	@echo "###"
	@echo "# Run:"
	@echo "# sudo update-rc.d aquacc defaults"
	@echo "# If you want aquacc to start automatically at boot."
	@echo "###"
