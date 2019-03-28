# Makefile for sw

#LDFLAGS = -I/usr/share/swsoc8/include/ -I../lib/
LDFLAGS = -I../../swsoc8/include/ -I../lib/
LOADLIBS =
#LDLIBS = /usr/share/swsoc8/lib/swsoc_lib.o ../lib/rmap_lib.o ../lib/tpclib.o ../lib/tpcmath.o ../lib/trigio_lib.o -lm -lrt
LDLIBS = ../../swsoc8/lib/swsoc_lib.o ../lib/rmap_lib.o ../lib/tpclib.o ../lib/tpcmath.o ../lib/trigio_lib.o -lm -lrt
APS = status statusall reset de10reset flush flushall linkup linkdown
APS += regread regwrite set_clkdiv
APS += nodeini nodeini0 nodedef adcini
APS += get_data get_ped gettemp
APS += analped analdata analpeak analmwbuf
APS += port0read port3read fe_read
APS += port2reset port3reset softreset hardreset
APS += trigio_read trigio_rdone trigio_cntrst
#APS += chk_def set_def chk_busy chk_dready chk_trigio
APS += chk_def set_def chk_linkup
APS += set_fullrange
APS += getver
APS += makeped
APS += read32 write32 sw_speed

.c.o:
	$(CC) -Wall -c $*.c -o $@

## Aps

aps : $(APS)

analmwbuf: analmwbuf.c
	$(CC) -Wall -o analmwbuf analmwbuf.c

#
clean:
	rm -f *.o *~ $(APS)

