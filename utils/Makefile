# Makefile for sw

APS = analmwbuf

pcidev = $(shell ls -d /usr/share/swpci 2>/dev/null | grep swpci)
socdev = $(shell ls -d /usr/share/swsoc8 2>/dev/null | grep swsoc8)

ifeq (${pcidev},/usr/share/swpci)
devexist=yes
CFLAGS += -Wall -I/usr/share/swpci/include/ -I../lib/ -DPCIE
LDLIBS += /usr/share/swpci/lib/swpci_lib.o
endif

ifeq (${socdev},/usr/share/swsoc8)
devexist=yes
CFLAGS += -Wall -I/usr/share/swsoc8/include/ -I../lib/
LDLIBS += /usr/share/swsoc8/lib/swsoc_lib.o
endif



#ifeq ($(shell uname -m),armv7l)
#CFLAGS += -Wall -I/usr/share/swsoc8/include/ -I../lib/
#LDLIBS += /usr/share/swsoc8/lib/swsoc_lib.o
#endif
#ifeq ($(shell uname -m),x86_64)
#CFLAGS += -Wall -I/usr/share/swpci/include/ -I../lib/ -DPCIE
#LDLIBS += /usr/share/swpci/lib/swpci_lib.o
#endif

ifeq (${devexist},yes)
LDLIBS += ../lib/rmap_lib.o ../lib/tpclib.o ../lib/tpcmath.o ../lib/trigio_lib.o
LDLIBS += -lm -lrt
APS += status statusall reset flush flushall linkup linkdown
APS += regread regwrite set_clkdiv
APS += nodeini nodeini0 nodedef adcini
APS += get_data get_ped gettemp
APS += analped analdata analpeak
APS += port0read port3read fe_read
APS += port1reset port2reset port3reset softreset hardreset
APS += spwreset
APS += trigio_read trigio_rdone trigio_cntrst
#APS += chk_def set_def chk_busy chk_dready chk_trigio
APS += chk_def set_def chk_linkup
APS += set_fullrange
APS += set_clkhz
APS += getver
APS += makeped
APS += read32 write32 sw_speed sw_loopback_test
APS += de10reset
endif

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $@

## Aps

aps : $(APS)

analmwbuf: analmwbuf.c
	$(CC) -Wall -o analmwbuf analmwbuf.c

#
clean:
	rm -f *.o *~ $(APS)

