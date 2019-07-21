COMP_NAME = DumGenA

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# LEPS2 DAQ common
CXXFLAGS += -I/home/online/local/include/
OBJS += ../lib/daqmwlib.o
CPPFLAGS += -I../lib/ -g
#LDLIBS += -lz -llz4
LDLIBS += -lz ../../local/lib/liblz4.a ../../local/lib/libzstd.a

INCS  = read_data_from_detectors.inc
INCS += set_data.inc

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
