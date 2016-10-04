INCLUDE=-I../include
GCCROOT=/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin
CFLAGS=$(INCLUDE) -std=c11
CC=$(GCCROOT)/armv7l-tizen-linux-gnueabi-gcc

all: test

test: test.c
	@echo [Arm-cc] $<...
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm test
