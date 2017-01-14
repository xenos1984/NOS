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
* i[4567]86-pc-elf (active)
* x86_64-pc-elf (temporarily not supported)
* m68k-atari-elf (temporarily not supported)
* arm-raspi2-eabi (planned)

## Latest builds:
* [i786-pc-elf GRUB2 bootable iso image](https://circleci.com/api/v1/project/xenos1984/NOS/latest/artifacts/0/$CIRCLE_ARTIFACTS/i786-pc-elf/NOS.iso) ([config log](https://circleci.com/api/v1/project/xenos1984/NOS/latest/artifacts/0/$CIRCLE_ARTIFACTS/i786-pc-elf/config.txt), [build log](https://circleci.com/api/v1/project/xenos1984/NOS/latest/artifacts/0/$CIRCLE_ARTIFACTS/i786-pc-elf/build.txt))

