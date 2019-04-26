# Makefile for sw

LIBS=daqmwlib.o

pcidev = $(shell ls /usr/share | grep swpci)
socdev = $(shell ls /usr/share | grep swsoc8)

ifeq (${pcidev},swpci)
CFLAGS += -Wall -I/usr/share/swpci/include/ -DPCIE -g
CFLAGS += -DDMA
LIBS += rmap_lib.o tpclib.o trigio_lib.o tpcmath.o
endif

ifeq (${socdev},swsoc8)
CFLAGS += -Wall -I/usr/share/swsoc8/include/ -g
LIBS += rmap_lib.o tpclib.o trigio_lib.o tpcmath.o
endif

#ifeq ($(shell uname -m),x86_64)
#CFLAGS += -Wall -I/usr/share/swpci/include/ -DPCIE -g
#CFLAGS += -DDMA
#endif
#ifeq ($(shell uname -m),armv7l)
#CFLAGS += -Wall -I/usr/share/swsoc8/include/ -g
#endif
#ifeq ($(shell uname -i),unknown)
#LIBS=daqmwlib.o
#else
#LIBS=rmap_lib.o tpclib.o trigio_lib.o daqmwlib.o tpcmath.o
#endif

#libs: rmap_lib.o tpclib.o trigio_lib.o daqmwlib.o tpcmath.o
libs: $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $@

clean:
	rm -f *.o *~

