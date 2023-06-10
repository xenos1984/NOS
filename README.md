[![CircleCI](https://circleci.com/gh/xenos1984/NOS.svg?style=svg)](https://circleci.com/gh/xenos1984/NOS)

# NOS
Operating system project

## Design goals
* microkernel
* portability: targeting multiple architectures
* using latest C++ standards
* clean and maintainable codebase

## Compile instructions
*	Make sure you have a [GCC / Binutils cross compiler toolchain](http://wiki.osdev.org/GCC_Cross-Compiler) configured for the desired target triplet.
*	Download the source code:

		git clone https://github.com/xenos1984/NOS.git

*	Run the autotools script:

		./autogen.sh

*	Configure the target architecture (replace &lt;target triplet&gt; with the target triplet):

		./configure --host=<target-triplet>

*	Build:

		make

## Supported target triplets
* **i[3456]86-pc-elf** - requires toolchain targeting **i[3456]86-elf**
* **x86\_64-pc-elf** - requires toolchain targeting **x86\_64-elf**
* **arm-raspi1-eabi** - requires toolchain targeting **arm-eabi**
* **arm-raspi2-eabihf** - requires toolchain targeting **arm-eabihf**
* **aarch64-raspi3-elf** - requires toolchain targeting **aarch64-elf**
* **m68k-atari-elf** - requires toolchain targeting **m68k-elf** (temporarily not supported)

## Latest builds
A snapshot of the latest commit can be found as the [latest release](https://github.com/xenos1984/NOS/releases/tag/latest). It includes an automatically generated [build log](https://github.com/xenos1984/NOS/releases/download/latest/build.html) and [boot console output](https://github.com/xenos1984/NOS/releases/download/latest/output.html) for a number of target platforms.
