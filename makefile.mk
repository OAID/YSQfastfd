PREFIX = aarch64-linux-gnu-

CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
AR = $(PREFIX)ar
CP = cp

# ARMv8 (arm64) enable, comment it if on arm32
ONARM64 = 1

#FRAME supports 720P or 480P
#FRAME_720P = 1

INCDIR = $(TOPDIR)/include
LIBFCVDIR = $(TOPDIR)/libfcv
MLIBDIR = $(TOPDIR)/middle
3RDLIBS = $(TOPDIR)/3rdlibs
GTK-VERSION = gtk+-3.0

FCVLIB = $(LIBFCVDIR)/liboal.a

linksyslibs = -lm -lpthread `pkg-config --libs $(GTK-VERSION)`
linkfcvlibs = -L$(LIBFCVDIR) -loal
linkaaidlibs = -L$(MLIBDIR) -laaid

ifeq ($(ONARM64), 1)
 linkysqfdlibs = -L$(3RDLIBS) -lysqfd64
else
 linkysqfdlibs = -L$(3RDLIBS) -lysqfd32
endif

ifeq ($(FRAME_720P), 1)
 CFLAGS += -DCAM_FRAME_720P
 CXXFLAGS += -DCAM_FRAME_720P
else
 CFLAGS += -DCAM_FRAME_480P
 CXXFLAGS += -DCAM_FRAME_480P
endif

CFLAGS += `pkg-config --cflags $(GTK-VERSION)`
CFLAGS += -Wall -O2
CFLAGS += -I$(INCDIR)

CXXFLAGS += `pkg-config --cflags $(GTK-VERSION)`
CXXFLAGS += -Wall -O2
CXXFLAGS += -I$(INCDIR)

#CFLAGS +=-DDEBUG
#CXXFLAGS +=-DDEBUG

#CFLAGS += -DPERF_STATISTICS
#CXXFLAGS += -DPERF_STATISTICS

CFLAGS += -D__linux__
CXXFLAGS += -D_linux__

ARFLAGS = -rcv

LDFLAGS += $(linksyslibs)

%.i : %.c
	$(CC) $(CFLAGS) -E $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d : %.c
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed -e 's/\($(@F)\.o\)[ :]*/$(<D)\/\1 $(@D)\/$(@F) : /g' $@.$$$$ > $@ ; \
	$(RM) $@.$$$$;

%.i : %.cpp
	$(CXX) $(CXXFLAGS) -E $< -o $@

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.d : %.cpp
	@$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed -e 's/\($(@F)\.o\)[ :]*/$(<D)\/\1 $(@D)\/$(@F) : /g' $@.$$$$ > $@ ; \
	$(RM) $@.$$$$;

sinclude $(.depens)
