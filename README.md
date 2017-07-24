This is a demo use YSQ face detect algorithm.

Platform :
 Ubuntu 16.04 with GTK3 library
  USB camera (480P/720P), YUYV

algorithm :
  AArch32 shared libray

## Build
* Check the gtk+ devel library is installed
   "sudo apt install build-essential vim-gtk libgtk-3-0 libgtk-3-dev libegl1-mesa-dev"

* Build the runtime shared libraries
   "make"

* "sudo mkdir /usr/local/ysqfd"
   "sudo cp 3rdlibs/libysqfd.so /usr/local/ysqfd/."
   "sudo cp middle/libaaid.so /usr/local/ysqfd/."

   "export LD_LIBRARY_PATH=/usr/local/ysqfd"

* run "fddemo/ysqfddemo"

NOTE :
  Type key 'q' to quit the display window.
