TOPDIR = $(shell pwd)

export TOPDIR

include $(TOPDIR)/makefile.mk

LIBDIRS = $(LIBFCVDIR)
LIBDIRS += $(MLIBDIR)

APPDIRS = $(TOPDIR)/fcvlibtest
APPDIRS += $(TOPDIR)/fddemo

all : libs apps

force:
# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.

libs : force
	@for dir in $(LIBDIRS); do \
		$(MAKE) -C $$dir;      \
	done

apps : libs
	@for dir in $(APPDIRS); do \
		$(MAKE) -C $$dir;      \
	done

clean : libclean appclean

libclean :
	@for dir in $(LIBDIRS); do \
		$(MAKE) -C $$dir clean ;  \
	done
	$(RM) $(EXES)
	@find . -name "*.[od]" | xargs rm -f

appclean :
	@for dir in $(APPDIRS); do \
		$(MAKE) -C $$dir clean ;  \
	done
	$(RM) $(EXES)
	@find . -name "*.[od]" | xargs rm -f

.PHONY : all clean force libs
