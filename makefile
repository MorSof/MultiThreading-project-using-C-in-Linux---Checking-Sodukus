CC=gcc
OBJS=v1.o v2.o
EXEC=v1 v2
CFLAGS = -std=c99  
LDFLAGS=-pthread 
all: $(EXEC)

v1: v1.o
	$(CC) -o $@ $^ $(LDFLAGS)

v2: v2.o

v1.c: v1.h
v2.c: v2.h

clean: 
	rm -f $(EXEC) $(OBJS)
	

