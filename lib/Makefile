# Makefile for sw

ifeq ($(shell uname -m),x86_64)
CFLAGS += -Wall -I/usr/share/swpci/include/ -DPCIE -g
CFLAGS += -DDMA
endif
ifeq ($(shell uname -m),armv7l)
CFLAGS += -Wall -I/usr/share/swsoc8/include/ -g
endif

libs: rmap_lib.o tpclib.o trigio_lib.o daqmwlib.o tpcmath.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $@

clean:
	rm -f *.o *~

