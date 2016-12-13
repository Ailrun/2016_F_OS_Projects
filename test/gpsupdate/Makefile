include ../Config.mk

TAR=gpsupdate
SRC=gpsupdate.c

.PHONY: all clean

all: $(TAR)

$(TAR): $(SRC)
	@echo [Arm-cc] $<...
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(TAR)
