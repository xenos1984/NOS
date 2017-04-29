#!/bin/bash
set -e

target=$1
prefix=~/cross/$target

gcc_version=`wget -O - ftp://ftp.gnu.org/gnu/gcc/ | grep -o 'gcc-[0-9]\+.[0-9]\+.[0-9]\+' | sed 's/gcc-//' | sort -V | tail -n 1`
binutils_version=`wget -O - ftp://ftp.gnu.org/gnu/binutils/ | grep -o 'binutils-[0-9]\+.[0-9]\+\(.[0-9]\+\)\?' | sed 's/binutils-//' | sort -V | tail -n 1`
newlib_version="2.4.0"

echo "Latest gcc version: $gcc_version"
echo "Latest binutils version: $binutils_version"

if [ -d $prefix ]
then
	gcc_current=`$target-gcc -v 2>&1 | grep -o '[0-9]\+.[0-9]\+.[0-9]\+' | sort -V | tail -1`
	binutils_current=`$target-ld -v 2>&1 | grep -o '[0-9]\+.[0-9]\+\(.[0-9]\+\)\?' | sort -V | tail -1`

	echo "Current gcc version: $gcc_current"
	echo "Current binutils version: $binutils_current"

	if [ $gcc_version != $gcc_current ] || [ $binutils_version != $binutils_current ]
	then
		echo "Need to update!"
		rm -rf $prefix
	fi
fi

if [ ! -d $prefix ]
then
	mkdir -p /tmp/toolchain
	cd /tmp/toolchain

	if [ ! -f gcc-$gcc_version.tar.bz2 ]
	then
		wget -c -O gcc-$gcc_version.tar.bz2 ftp://ftp.gnu.org/gnu/gcc/gcc-$gcc_version/gcc-$gcc_version.tar.bz2
		tar -xf gcc-$gcc_version.tar.bz2
	fi

	if [ ! -f binutils-$binutils_version.tar.bz2 ]
	then
		wget -c -O binutils-$binutils_version.tar.bz2 ftp://ftp.gnu.org/gnu/binutils/binutils-$binutils_version.tar.bz2
		tar -xf binutils-$binutils_version.tar.bz2
	fi

	if [ ! -f newlib-$newlib_version.tar.gz ]
	then
		wget -c -O newlib-$newlib_version.tar.gz ftp://sources.redhat.com/pub/newlib/newlib-$newlib_version.tar.gz
		tar -xf newlib-$newlib_version.tar.gz
	fi

	mkdir -p /tmp/toolchain/build-binutils
	mkdir -p /tmp/toolchain/build-gcc
	mkdir -p /tmp/toolchain/build-newlib

	cd /tmp/toolchain/build-binutils
	sudo rm -rf *
	/tmp/toolchain/binutils-$binutils_version/configure --target=$target --prefix=$prefix --disable-nls 2>&1
	make all 2>&1
	make install 2>&1
	sudo rm -rf *

	cd /tmp/toolchain/build-gcc
	/tmp/toolchain/gcc-$gcc_version/configure --target=$target --prefix=$prefix --disable-nls --enable-languages=c,c++ --enable-libstdcxx --without-headers 2>&1
	make all-gcc 2>&1
	make install-gcc 2>&1
	make all-target-libgcc 2>&1
	make install-target-libgcc 2>&1

	sudo ln -s -f $prefix/bin/* /usr/local/bin/

	cd /tmp/toolchain/build-newlib
	sudo rm -rf *
	/tmp/toolchain/newlib-$newlib_version/configure --target=$target --prefix=$prefix 2>&1
	make all 2>&1
	make install 2>&1
	sudo rm -rf *

	cd /tmp/toolchain/build-gcc
	/tmp/toolchain/gcc-$gcc_version/configure --target=$target --prefix=$prefix --disable-nls --enable-languages=c,c++ --enable-libstdcxx --without-headers --with-newlib 2>&1
	make all-target-libstdc++-v3 2>&1
	make install-target-libstdc++-v3 2>&1
	sudo rm -rf *
fi

sudo ln -s -f $prefix/bin/* /usr/local/bin/

