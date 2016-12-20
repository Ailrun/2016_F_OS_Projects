TEST_PROGS=gpsupdate file_loc

include Misc.mk

.PHONY: all clean

all:
	@echo ""
	@echo ""
	@for tprog in $(TEST_PROGS); do \
		echo $(HZ_DIVIDER); \
		printf "%20sbuild %s\n" "" "$$tprog"; \
		$(MAKE) $(MAKEFLAGS) -C $$tprog; \
		printf "%20s%s done\n" "" "$$tprog"; \
	done
	@echo $(HZ_DIVIDER)
	@echo ""
	@echo ""

clean:
	@echo ""
	@echo ""
	@for tprog in $(TEST_PROGS); do \
		echo $(HZ_DIVIDER); \
		printf "%20sclean %s\n" "" "$$tprog"; \
		$(MAKE) $(MAKEFLAGS) -C $$tprog clean; \
		printf "%20s%s cleaned\n" "" "$$tprog"; \
	done
	@echo $(HZ_DIVIDER)
	@echo ""
	@echo ""
