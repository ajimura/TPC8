COMP_NAME = TPCreaderZ

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
OBJS += ../lib/daqmwlib.o

# for space-wire/rmap
ifeq ($(shell uname -m),x86_64)
OBJS += /usr/share/swpci/lib/swpci_lib.o
CPPFLAGS += -I/usr/share/swpci/include/ -g
endif
ifeq ($(shell uname -m),armv7l)
OBJS += /usr/share/swsoc8/lib/swsoc_lib.o
CPPFLAGS += -I/usr/share/swsoc8/include/ -g
endif
OBJS += ../lib/rmap_lib.o
CPPFLAGS += -I../lib/

# for TPC readout
OBJS += ../lib/tpclib.o ../lib/trigio_lib.o
INCS = daq_configure.inc daq_pause.inc daq_resume.inc daq_start.inc daq_stop.inc parse_params.inc read_data_from_detectors.inc
TPCreaderZ.o : TPCreaderZ.cpp $(INCS)

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
