# Makefile for sw

LDFLAGS = -I/usr/share/swsoc8/include/
LOADLIBS =
LDLIBS = 
APS = 

libs: rmap_lib.o tpclib.o trigio_lib.o daqmwlib.o tpcmath.o

rmap_lib.o:  rmap_lib.c
	$(CC) -c $(LDFLAGS) rmap_lib.c

tpclib.o: tpclib.c tpclib.h
	cc -g -c $(LDFLAGS) tpclib.c

tpcmath.o: tpcmath.c tpcmath.h
	cc -g -c tpcmath.c

trigio_lib.o: trigio_lib.c
	cc -g -c trigio_lib.c

daqmwlib.o: daqmwlib.c
	cc -g -c daqmwlib.c

clean:
	rm -f *.o *~

