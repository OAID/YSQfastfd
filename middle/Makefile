SLIBAAID = libaaid.so

slib-objs += midlayer.o

.depens := $(patsubst %.o, %.d, $(slib-objs))

CXXFLAGS += -fpic -shared

.PHONY : all clean
all : $(SLIBAAID) movefile

$(SLIBAAID) : $(slib-objs)
	$(CXX) $(CXXFLAGS) -o $@ $^

movefile : $(SLIBAAID)
	$(CP) $(SLIBAAID) ../3rdlibs/

clean :
	$(RM) $(SLIBAAID) *.o *.d *.i

include $(TOPDIR)/makefile.mk
