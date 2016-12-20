include ../Config.mk

TAR=file_loc
SRC=file_loc.c

.PHONY: all clean

all: $(TAR)

$(TAR): $(SRC)
	@echo [Arm-cc] $<...
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(TAR)

