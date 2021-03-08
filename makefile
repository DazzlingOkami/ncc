CC = gcc
CFLAGES = -g -Wall -DNDEBUG -D_CONSOLE -DGAPING_SECURITY_HOLE
CFLAGESL = -s -O2 -Wall -DNDEBUG -D_CONSOLE
LDFLAGS = -lwsock32

all : ncc

ncc : ncc.c doexec.c
	$(CC) $(CFLAGES) -o $(@) ncc.c doexec.c $(LDFLAGS)

nccl : ncc.c
	$(CC) $(CFLAGESL) -o $(@) ncc.c $(LDFLAGS)
	size nccl.exe

clean :
	del *.exe
