include ../Config.mk

TAR=selector
SRC=selector.c

all: $(TAR)

$(TAR): $(SRC)
	@echo [Arm-cc] $<...
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(TAR)
