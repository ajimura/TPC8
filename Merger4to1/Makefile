COMP_NAME = Merger4to1

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
OBJS += ../lib/daqmwlib.o
CPPFLAGS += -I../lib/ -g

Merger4to1.o : Stock_data.inc daq_run.inc read_InPort1.inc read_InPort2.inc read_InPort3.inc read_InPort4.inc

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
