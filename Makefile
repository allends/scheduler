CC = gcc
CFLAGS = -g -c
AR = ar -rc
RANLIB = ranlib

SCHED = RR

all: queue.o mypthread.a

queue.o: queue.h
	$(CC) $(CFLAGS) queue.c

mypthread.a: mypthread.o queue.o
	$(AR) libmypthread.a mypthread.o queue.o
	$(RANLIB) libmypthread.a

mypthread.o: mypthread.h

ifeq ($(SCHED), RR)
	$(CC) -pthread $(CFLAGS) mypthread.c
else ifeq ($(SCHED), PSJF)
	$(CC) -pthread $(CFLAGS) -DPSJF mypthread.c
else ifeq ($(SCHED), MLFQ)
	$(CC) -pthread $(CFLAGS) -DMLFQ mypthread.c
else
	echo "no such scheduling algorithm"
endif

clean:
	rm -rf testfile *.o *.a
