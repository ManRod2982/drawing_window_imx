# Drawing window

This is a simple window that allows to write in black and white.
It works with [SimpleNN](https://github.com/ManRod2982/SimpleNN) and a pre-trained neural network to identify the digit that has been drawn.

![alt text](https://github.com/ManRod2982/drawing_window/raw/master/docs/window_example.png "Nmist window")

# Building the project

The project is built using CMake and it requires a toolchain with GTKMM3 support.

```
source /opt/fsl-imx-xwayland/6.12-walnascar-full-gtkmm3/environment-setup-armv8a-poky-linux
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$OECORE_NATIVE_SYSROOT/usr/share/cmake/OEToolchainConfig.cmake
cmake --build build
```

And the target binary will be built and can be executed as follows

`./build/window`

# Dependencies

This project uses CMake 3.10 or later and Gtkmm 3.0

```
sudo apt install cmake
sudo apt install libgtkmm-3.0-dev
sudo apt-get install libeigen3-dev
```

It also depends on [SimpleNN](https://github.com/ManRod2982/SimpleNN) it can be cloned in placed in the same folder for cmake to add it as a subdirectory.

# License

This software is available under the MIT license.
