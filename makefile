CC = gcc
CFLAGES = -g -Wall -DNDEBUG -D_CONSOLE
CFLAGESL = -s -O2 -Wall -DNDEBUG -D_CONSOLE
LDFLAGS = -lwsock32

all : ncc

ncc : ncc.c
	$(CC) $(CFLAGES) -o $(@) ncc.c $(LDFLAGS)

nccl : ncc.c
	$(CC) $(CFLAGESL) -o $(@) ncc.c $(LDFLAGS)
	size nccl.exe

clean :
	del *.exe
