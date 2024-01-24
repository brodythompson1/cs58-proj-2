.SUFFIXES: .c

SRCS = bridge.c
OBJS = $(SRCS:.c=.o)
OUTPUT = bridge

CC = gcc
CFLAGS = -Wall -D_POSIX_C_SOURCE -pedantic -std=c11 -ggdb
LIBS = -lm

bridge: $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS) $(LIBS)


clean:
	rm -f $(OBJS) $(OUTPUT)

depend:
	makedepend -I/usr/local/include/g++ -- $(CFLAGS) -- $(SRCS) 