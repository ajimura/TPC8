COMP_NAME = TPCreaderB

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
OBJS += ../lib/daqmwlib.o

# for space-wire/rmap
OBJS += /usr/share/swsoc8/lib/swsoc_lib.o ../lib/rmap_lib.o
CPPFLAGS += -I../lib/ -I/usr/share/swsoc8/include/ -I../TPCreader/ -g

# for TPC readout
OBJS += ../lib/tpclib.o ../lib/trigio_lib.o

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
