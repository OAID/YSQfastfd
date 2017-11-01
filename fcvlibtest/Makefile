include $(TOPDIR)/makefile.mk

EXES = libfcvtest

LDFLAGS += $(linkfcvlibs)
LDFLAGS += $(linksyslibs)

all : $(EXES)

libfcvtest : libfcvtest.cpp $(FCVLIB)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

clean :
	$(RM) $(EXES)

.PHONY : all clean
