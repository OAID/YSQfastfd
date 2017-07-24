include $(TOPDIR)/makefile.mk

LDFLAGS += $(linkfcvlibs)
LDFLAGS += $(linkaaidlibs)
LDFLAGS += $(linkysqfdlibs)
LDFLAGS += $(linksyslibs)

EXES = ysqfddemo

all : $(EXES)

ysqfddemo : ysqfddemo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean :
	$(RM) $(EXES)

.PHONY : all clean
