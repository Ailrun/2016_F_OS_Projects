.PHONY: all clean

all:
	$(MAKE) -C rotd
	$(MAKE) -C selector
	$(MAKE) -C trial

clean:
	$(MAKE) -C rotd clean
	$(MAKE) -C selector clean
	$(MAKE) -C trial clean
	@rm -f *~
	@rm -f */*~
