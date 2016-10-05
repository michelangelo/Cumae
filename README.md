# Cumae

Cumae is a tiny software layer in plain C that is meant to act as a realtime OS for AVR-based devices.

SBuCa (*Smart Business Card*) is powered by Cumae; SBuCa makes use of a G2 controller that drives a 1.44" EPD display. As you may expect the whole thing is based around an AVR.

## Build and Use

Cumae is a self-contained project that includes the whole gcc-avr toolchain, along with avr-libc and avrdude.

### Requirements
The usual one-liner for Ubuntu 16.04 is the following
```
sudo apt install build-essentials git texinfo flex bison libgmp-dev libmpfr-dev libmpc-dev libusb-dev libusb-1.0-0-dev libelf-dev libftdi-dev libftdi1-dev
```
### Bootstrap the Toolchain
The first thing is to clone this repository; do that by cloning the repository with `git clone https://github.com/michelangelo/Cumae.git --recursive`.
If you have already cloned the repository *without* appending the `--recursive` parameter, you can update all the submodules by running:
```
git submodule init
```
Followed by
```
git submodule update
```

Now that you have all the submodules nice and ready, you can build them all with:
```
tools/build/build-toolchain.sh
```
This is gonna take a while but if everything goes well you'll have all the tools in the `out/` directory.

### Build the main Cumae app
To build the main Cumae app just `cd src/main` and run `make`. Everything should build smoothly.

### Flashing
Pay attention to the file `Makefile.in` in the root of the repository for all the  information about the target MCU and clock settings. The project default is for an atmega328p with a clock of 12MHz driven by a full swing crystal. An Atmel ICE is used as flasher (SPI).

Anyway, in order to flash your MCU you'll just need to issue `make flash` from the directory `src/main`.

### Misc
You can add your own *app* on top of Cumae by simply creating a new sub-directory under `src/`. A `Makefile` **needs to be present** in every directory; by doing so you'll be reusing all the rules Cumae provides, such as the one to flash your MCU. Your `Makefile` only needs to define one target. Check the example in `src/blink`.
