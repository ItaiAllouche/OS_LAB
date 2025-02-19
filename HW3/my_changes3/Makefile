CC = gcc
CFLAGS += -I/usr/src/linux-2.4.18-14custom/include -Wall
OBJS = vegenere.o

all: $(OBJS)
	gcc -o test_huge test_huge.c
    
clean:
	rm -f *.o *~ test_huge
