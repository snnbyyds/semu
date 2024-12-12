include $(SEMU_HOME)/scripts/build.mk

override ARGS ?=

SEMU_EXEC := $(BINARY) $(ARGS)

run-env: $(BINARY)

run: run-env
	$(SEMU_EXEC)

gdb: run-env
	gdb -s $(BINARY) --args $(SEMU_EXEC)

clean-all: clean distclean

.PHONY: run gdb run-env clean-all
