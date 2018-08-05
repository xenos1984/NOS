[![CircleCI](https://circleci.com/gh/xenos1984/NOS.svg?style=svg)](https://circleci.com/gh/xenos1984/NOS)

# NOS
Operating system project

## Compile instructions
*	Make sure you have a [GCC / Binutils cross compiler toolchain](http://wiki.osdev.org/GCC_Cross-Compiler) configured for the desired target triplet.
*	Download the source code:

		git clone https://github.com/xenos1984/NOS.git

*	Run the autotools script:

		./autogen.sh

*	Configure the target architecture (replace &lt;target triplet&gt; with the target triplet):

		./configure <target-triplet>

*	Build:

		make

## Supported target triplets:
* i[3456]86-pc-elf (active)
* x86\_64-pc-elf (active)
* m68k-atari-elf (temporarily not supported)
* arm-raspi2-eabi (active)
* aarch64-raspi3-elf (active)

## Latest builds:
* [i686-pc-elf GRUB2 bootable iso image](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=i686-pc-elf/mp-acpi/NOS.iso) ([config log](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=i686-pc-elf/mp-acpi/config.log), [build log](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=i686-pc-elf/mp-acpi/build.log))
* [x86\_64-pc-elf GRUB2 bootable iso image](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=x86_64-pc-elf/mp-acpi/NOS.iso) ([config log](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=x86_64-pc-elf/mp-acpi/config.log), [build log](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=x86_64-pc-elf/mp-acpi/build.log))
* [arm-raspi2-eabi ELF kernel](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=arm-raspi2-eabi/mp/Kernel.elf) ([config log](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=arm-raspi2-eabi/mp/config.log), [build log](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=arm-raspi2-eabi/mp/build.log))
* [aarch64-raspi3-elf ELF kernel](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=aarch64-raspi3-elf/mp/Kernel.elf) ([config log](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=aarch64-raspi3-elf/mp/config.log), [build log](http://manuelhohmann.ddns.net/osdev/nosrelease.php?file=aarch64-raspi3-elf/mp/build.log))

