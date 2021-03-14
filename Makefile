CC = gcc -g
CFLAGS = -Og -Wall -Werror -std=gnu99

OBJS = traceroute.o packets.o utils.o

FILES = traceroute.c packets.h packets.c utils.h utils.c

all: $(OBJS) 
	$(CC) $(CFLAGS) -o traceroute $(OBJS)

traceroute.o: traceroute.c
		$(CC) $(CFLAGS) -c traceroute.c

utils.o: utils.h utils.c
		$(CC) $(CFLAGS) -c utils.h utils.c
packets.o: packets.h packets.c	
		$(CC) $(CFLAGS) -c packets.h packets.c

clean: 
		rm -f *.o *.gch

distclean: 
		rm -f *.o *.gch traceroute
