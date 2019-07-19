COMP_NAME = Merger2to1

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
OBJS += ../lib/daqmwlib.o
CPPFLAGS += -I../lib/ -g
#LDLIBS += -lz -llz4
LDLIBS += -lz ../../local/lib/liblz4.a

INCS  = read_InPort1.inc
INCS += read_InPort2.inc
INCS += Stock_data.inc
INCS += daq_run.inc
INCS += set_data.inc

Merger2to1.o: $(INCS)

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
