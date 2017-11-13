 YSQ face detect algorithm demo on ARM (Linux) platform.

[![License](https://img.shields.io/hexpm/l/plug.svg)](LICENSE)

## Performance on ARM platform 

Test on Allwinner R40 (ARM32) and Rockchip RK3399 (ARM64)

### Allwinner R40 A7 platform

![R40_perf_two_face](https://oaid.github.io/pics/YSQfd/R40_perf.png) 

### Rockchip RK3399 A72 platform

![RK3399_perf_two_face](https://oaid.github.io/pics/YSQfd/RK3399_perf.png)

## Platform preparation :

* ARM32 platform hardware board 
* Ubuntu 16.04 with GTK3 library
* USB camera (480P/720P), YUYV

## Build
#### Check the gtk+ devel library is installed

	sudo apt install build-essential vim-gtk libgtk-3-0 libgtk-3-dev libegl1-mesa-dev

#### Build the runtime shared libraries

	cd YSQfastfd
	make -j4

#### run the demo

	chmod +x ./run.sh
        ./run.sh

NOTE :
* Type key 'q' to quit the display window.
* Type key 's' to show the landmark info.
* Type key 'p' to save the face picture.
