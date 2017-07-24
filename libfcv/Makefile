LIBOAL = liboal.a

lib-objs += video.o
lib-objs += yuvproc.o
lib-objs += window.o
lib-objs += fcvimage.o
lib-objs += queue.o
lib-objs += aaidsys.o
lib-objs += ysqfd.o

.depens := $(patsubst %.o, %.d, $(lib-objs))

.PHONY : all clean
all : $(LIBOAL)

$(LIBOAL) : $(lib-objs)
	$(AR) $(ARFLAGS) $@ $^

clean :
	$(RM) $(LIBOAL) *.o *.d *.i

include $(TOPDIR)/makefile.mk
