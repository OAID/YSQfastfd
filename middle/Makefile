SLIBAAID = libaaid.so

slib-objs += midlayer.o

.depens := $(patsubst %.o, %.d, $(slib-objs))

CXXFLAGS += -fpic -shared

.PHONY : all clean
all : $(SLIBAAID)

$(SLIBAAID) : $(slib-objs)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean :
	$(RM) $(SLIBAAID) *.o *.d *.i

include $(TOPDIR)/makefile.mk
