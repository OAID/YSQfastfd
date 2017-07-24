include $(TOPDIR)/makefile.mk

EXES = libfcvtest

LDFLAGS += $(linkfcvlibs)
LDFLAGS += $(linksyslibs)

all : $(EXES)

libfcvtest : libfcvtest.c $(FCVLIB)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean :
	$(RM) $(EXES)

.PHONY : all clean
