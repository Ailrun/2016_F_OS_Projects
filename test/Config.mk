TOOLBASEDIR=

GCCROOT=${TOOLBASEDIR}/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin
CC=$(GCCROOT)/armv7l-tizen-linux-gnueabi-gcc

INCLUDE=-I../../include
CFLAGS=$(INCLUDE)
