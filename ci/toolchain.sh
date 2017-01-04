#!/bin/sh
set -e

binutils_version="2.27"
gcc_version="6.3.0"
newlib_version="2.4.0"

target=$1

if [ -d /usr/cross/$target ]
	exit 0
fi

mkdir -p /tmp/toolchain
cd /tmp/toolchain

if [ ! -f gcc-$gcc_version.tar.bz2 ]
	wget -c -O gcc-$gcc_version.tar.bz2 ftp://ftp.gnu.org/gnu/gcc/gcc-$gcc_version/gcc-$gcc_version.tar.bz2
	tar -xf gcc-$gcc_version.tar.bz2
fi

if [ ! -f binutils-$binutils_version.tar.bz2 ]
	wget -c -O binutils-$binutils_version.tar.bz2 ftp://ftp.gnu.org/gnu/binutils/binutils-$binutils_version.tar.bz2
	tar -xf binutils-$binutils_version.tar.bz2
fi

if [ ! -f newlib-$newlib_version.tar.gz ]
	wget -c -O newlib-$newlib_version.tar.gz ftp://sources.redhat.com/pub/newlib/newlib-$newlib_version.tar.gz
	tar -xf newlib-$newlib_version.tar.gz
fi

mkdir -p /tmp/toolchain/build-binutils
mkdir -p /tmp/toolchain/build-gcc
mkdir -p /tmp/toolchain/build-newlib

cd /tmp/toolchain/build-binutils
sudo rm -rf *
/tmp/toolchain/binutils-$binutils_version/configure --target=$target --prefix=/usr/cross/$target --disable-nls 2>&1
make all 2>&1
sudo make install 2>&1
sudo rm -rf *

cd /tmp/toolchain/build-gcc
/tmp/toolchain/gcc-$gcc_version/configure --target=$target --prefix=/usr/cross/$target --disable-nls --enable-languages=c,c++ --enable-libstdcxx --without-headers 2>&1
make all-gcc 2>&1
sudo make install-gcc 2>&1
make all-target-libgcc 2>&1
sudo make install-target-libgcc 2>&1

sudo ln -s -f /usr/cross/$target/bin/* /usr/local/bin/

cd /tmp/toolchain/build-newlib
sudo rm -rf *
/tmp/toolchain/newlib-$newlib_version/configure --target=$target --prefix=/usr/cross/$target 2>&1
make all 2>&1
sudo make install 2>&1
sudo rm -rf *

cd /tmp/toolchain/build-gcc
/tmp/toolchain/gcc-$gcc_version/configure --target=$target --prefix=/usr/cross/$target --disable-nls --enable-languages=c,c++ --enable-libstdcxx --without-headers --with-newlib 2>&1
make all-target-libstdc++-v3 2>&1
sudo make install-target-libstdc++-v3 2>&1
sudo rm -rf *

sudo ln -s -f /usr/cross/$target/bin/* /usr/local/bin/

