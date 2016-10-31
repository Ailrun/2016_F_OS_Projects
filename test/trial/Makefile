include ../Config.mk

TAR=trial
SRC=trial.c

all: $(TAR)

$(TAR): $(SRC)
	@echo [Arm-cc] $<...
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(TAR)
