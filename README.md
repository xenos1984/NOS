[![CircleCI](https://circleci.com/gh/xenos1984/NOS.svg?style=svg)](https://circleci.com/gh/xenos1984/NOS)

# NOS
Operating system project

## Compile instructions
*	Download the source code:

		git clone https://github.com/xenos1984/NOS.git

*	Run the autotools script:

		./autogen.sh

*	Configure the target architecture (replace &lt;target triplet&gt; with the target triplet):

		./configure <target-triplet>

*	Build:

		make

## Supported target triplets:
* i686-pc-elf (active)
* i786-pc-elf (active)
* x86_64-pc-elf (temporarily not supported)
* m68k-atari-elf (temporarily not supported)
* arm-raspi2-eabi (planned)

