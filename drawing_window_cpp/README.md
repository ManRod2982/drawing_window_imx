# Drawing window C++

This is a simple window that allows to write a digit in black and white and use a pre-trained Tensorflow lite model to
identify the digit. Using C++ with GTKMM and the tensorflow lite API.

![alt text](../imgs/window_example.png "Mnist window")

# Building the project

The project is built using CMake and it requires a toolchain with GTKMM3 support. In order to generate the toolchain
the following needs to be added to the local.conf file:

```
IMAGE_INSTALL:append = " gtkmm3"
```

Also recommended to add the following to avoid the build crashing due to out of memory errors:
```
BB_NUMBER_THREADS="8"
PARALLEL_MAKE="-j8"
BB_PRESSURE_MAX_CPU ?= "50000"
BB_PRESSURE_MAX_IO ?= "100000"
BB_PRESSURE_MAX_MEMORY ?= "25000"
```

After modifying the local.conf the image and the SDK can be created, to generate the SDK:

```
bitbake imx-image-full -c populate_sdk
```

And to generate the image:

```
bitbake imx-image-full
```
Note that up to 500GB are consumed during the build process.

Once the toolchain has been generated it can be installed as follows:

```
./tmp/deploy/sdk/fsl-imx-xwayland-glibc-x86_64-imx-image-full-armv8a-imx93-11x11-lpddr4x-frdm-toolchain-6.12-walnascar.sh
```

And the image can be flashed onto an sd card as follows:

```
zstdcat imx-image-full-imx93-11x11-lpddr4x-frdm.rootfs.wic.zst | sudo dd of=/dev/mmcblk0 bs=1M conv=fsync
```

To build the project the toolchain needs to be sourced:

```
source /opt/fsl-imx-xwayland/6.12-walnascar-full-gtkmm3/environment-setup-armv8a-poky-linux
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$OECORE_NATIVE_SYSROOT/usr/share/cmake/OEToolchainConfig.cmake
cmake --build build/window
```

And the target binary will be built for ARM64, now it can be simply copied onto the target by `scp` or just copy it to the sd card
on a Linux file system:

```
sudo cp build/window /media/user/root/root/
```
# Running the application

Boot the target and you should be able to execute the binary, the application requires:

- Model path: `-m model_path`

and has two optional parameters:
- Delegate library path: `-d delegate_path`
- Verbosity, without this option verbosity is disabled: '-v'

The following are some examples.

Tensorflite model, no quantization and no delegate, hence XNN will be used:
```
./window -m cnn.tflite -v
```
Ethos model with the ethos delegate and verbosity enabled
```
./window -m cnn_quant_vela.tflite -d /usr/lib/libethosu_delegate.so -v
```

# Dependencies

This project uses CMake 3.10 and requires a toolchain for the ARM64 (see above) with GTKMM3 and TFLite support.

```
sudo apt install cmake
```

# Model examples

[Models](../models/) are provided for creating example Tensorflow lite models using both Tensorflow/keras and pytorch.

# License

This software is available under the MIT license.
