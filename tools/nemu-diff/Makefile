REPO_PATH = $(shell pwd)/repo

LIBNEMU = $(REPO_PATH)/build/riscv32-nemu-interpreter-so

$(REPO_PATH)/.git:
	@if [ ! -d "$(REPO_PATH)" ]; then \
		git clone --depth=1 -b master https://github.com/ics-ext/nemu-ref.git $(REPO_PATH) || { echo "Failed to clone"; exit 1; }; \
	fi

repo/include/difftest-def.h: $(REPO_PATH)/.git

$(LIBNEMU): repo/include/difftest-def.h
	cd $(REPO_PATH) && make NEMU_HOME=$(REPO_PATH) riscv32-ref_defconfig && make NEMU_HOME=$(REPO_PATH)

clean:
	@if [ -d "$(REPO_PATH)" ]; then \
		$(MAKE) -C $(REPO_PATH) clean; \
	fi

.PHONY: all clean
.DEFAULT_GOAL := all
all: $(LIBNEMU)
