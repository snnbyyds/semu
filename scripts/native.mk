include $(SEMU_HOME)/scripts/build.mk
include $(SEMU_HOME)/tools/difftest.mk

override ARGS ?=

IMG ?=
SEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(SEMU_EXEC)

gdb: run-env
	gdb -s $(BINARY) --args $(SEMU_EXEC)

clean-all: clean distclean

.PHONY: run gdb run-env clean-all
