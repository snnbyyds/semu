ifeq ($(CONFIG_ENABLE_DIFFTEST),)
SRCS-BLACKLIST-y += src/utils/difftest/dut.c
else
NEMU_REF = tools/nemu-diff/repo/build/riscv32-nemu-interpreter-so

src/utils/difftest/dut.c: $(NEMU_REF)
$(NEMU_REF):
	$(MAKE) -C tools/nemu-diff
endif
