COMP_NAME = TPCreaderB
PARENT = ../TPCreader/

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
CXXFLAGS += -I/home/online/local/include/
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
CPPFLAGS += -I../lib/ -I../TPCreader/
#LDLIBS += -lz -llz4
LDLIBS += -lz ../../local/lib/liblz4.a ../../local/lib/libzstd.a

# for TPC readout
OBJS += ../lib/tpclib.o ../lib/trigio_lib.o

INCS  = $(PARENT)daq_configure.inc
INCS += $(PARENT)daq_pause.inc
INCS += $(PARENT)daq_resume.inc
INCS += $(PARENT)daq_start.inc
INCS += $(PARENT)daq_stop.inc
INCS += $(PARENT)daq_run.inc
INCS += $(PARENT)parse_params.inc
INCS += $(PARENT)read_data_from_detectors.inc
INCS += $(PARENT)set_data.inc

$(COMP_NAME).o : $(COMP_NAME).cpp $(INCS)

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
