# Makefile for sw

#LDFLAGS = -I/usr/share/swsoc8/include/
LDFLAGS = -I/home/online/swsoc8/include/
LOADLIBS =
LDLIBS = 
APS = 

libs: rmap_lib.o tpclib.o trigio_lib.o daqmwlib.o tpcmath.o

rmap_lib.o:  rmap_lib.c
	$(CC) -Wall -c $(LDFLAGS) rmap_lib.c

tpclib.o: tpclib.c address.h
	cc -Wall -g -c $(LDFLAGS) tpclib.c

tpcmath.o: tpcmath.c
	cc -Wall -g -c tpcmath.c

trigio_lib.o: trigio_lib.c
	cc -Wall -g -c trigio_lib.c

daqmwlib.o: daqmwlib.c
	cc -Wall -g -c daqmwlib.c

clean:
	rm -f *.o *~

