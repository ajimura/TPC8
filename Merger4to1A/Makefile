COMP_NAME = Merger4to1A
PARENT = ../Merger4to1/

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
OBJS += ../lib/daqmwlib.o
CPPFLAGS += -I../lib/ -I$(PARENT) -g

INCS  = $(PARENT)Stock_data.inc
INCS += $(PARENT)daq_run.inc
INCS += $(PARENT)read_InPort1.inc
INCS += $(PARENT)read_InPort2.inc
INCS += $(PARENT)read_InPort3.inc
INCS += $(PARENT)read_InPort4.inc

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
