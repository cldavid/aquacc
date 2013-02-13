CC	= gcc
CFLAGS += -g -Wall -Wextra
LDFLAGS += 

TARGET	= aquacc
SRCS	= main.c aquacc.c serial.c socket.c daemon.c
OBJS	= $(SRCS:.c=.o)


.PHONY: all clean install 

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(SRCS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:  
	rm -rf $(TARGET) $(OBJS) 

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
