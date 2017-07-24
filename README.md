This is a demo use YSQ face detect algorithm.

[![License](https://img.shields.io/badge/license-BSD-blue.svg)](LICENSE)

## Performance on ARM platform

### Allwinner R40 A7 platform
![R40_perf_two_face](https://OAID.github.io/pics/YSQfd/R40_perf.png) ![R40_perf_single_face](https://OAID.github.io/pics/YSQfd/R40_perf1.png)


### Rockchip RK3399 A72 platform
![R40_perf_two_face](https://OAID.github.io/pics/YSQfd/RK3399_perf.png) ![R40_perf_single_face](https://OAID.github.io/pics/YSQfd/RK3399_perf1.png )

## Platform preparation :

* ARM32 platform hardware board 
* Ubuntu 16.04 with GTK3 library
* USB camera (480P/720P), YUYV

## Build
* Check the gtk+ devel library is installed

		sudo apt install build-essential vim-gtk libgtk-3-0 libgtk-3-dev libegl1-mesa-dev

* Build the runtime shared libraries

		cd YSQfastfd
		make -j4

* Copy libraries 
 
		sudo mkdir /usr/local/ysqfd
		sudo cp 3rdlibs/libysqfd32.so /usr/local/ysqfd/.
		sudo cp 3rdlibs/libysqfd64.so /usr/local/ysqfd/.
		sudo cp middle/libaaid.so /usr/local/ysqfd/.
		export LD_LIBRARY_PATH=/usr/local/ysqfd

* run the demo

		fddemo/ysqfddemo

NOTE :
  Type key 'q' to quit the display window.
