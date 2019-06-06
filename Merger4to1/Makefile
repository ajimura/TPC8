COMP_NAME = Merger4to1

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
OBJS += ../lib/daqmwlib.o
CPPFLAGS += -I../lib/ -g

INCS  = read_InPort1.inc
INCS += read_InPort2.inc
INCS += read_InPort3.inc
INCS += read_InPort4.inc
INCS += Stock_data.inc
INCS += daq_run.inc

$(COMP_NAME).o : $(INCS)

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
