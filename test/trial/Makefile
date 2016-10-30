TOOLBASEDIR=

INCLUDE=-I../../include
GCCROOT=${TOOLBASEDIR}/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin
CFLAGS=$(INCLUDE)
CC=$(GCCROOT)/armv7l-tizen-linux-gnueabi-gcc

TAR=trial
SRC=trial.c

all: $(TAR)

$(TAR): $(SRC)
	@echo [Arm-cc] $<...
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(TAR)
