COMP_NAME = CMPRer

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
OBJS += ../lib/daqmwlib.o
CPPFLAGS += -I../lib/ -g
LDLIBS += -lz ../../local/lib/liblz4.a

INCS = daq_configure.inc
INCS += parse_params.inc
INCS += read_InPort.inc
INCS += set_data.inc
INCS += daq_run.inc

$(COMP_NAME).o: $(INCS)

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
