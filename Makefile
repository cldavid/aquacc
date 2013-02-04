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
	echo "INSTALL" 

