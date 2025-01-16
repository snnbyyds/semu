REPO_PATH = repo

ifeq ($(wildcard repo/source/include/softfloat.h),)
  $(shell git clone --depth=1 https://github.com/ucb-bar/berkeley-softfloat-3.git $(REPO_PATH))
endif

SOFTFLOAT = $(REPO_PATH)/build/Linux-x86_64-GCC/softfloat.a
$(SOFTFLOAT):
	cd $(REPO_PATH) && make SOFTFLOAT_OPTS="-fPIC -DSOFTFLOAT_ROUND_ODD -DINLINE_LEVEL=5 -DSOFTFLOAT_FAST_DIV32TO16 -DSOFTFLOAT_FAST_DIV64TO32 -O3" -C build/Linux-x86_64-GCC

clean:
	$(MAKE) -C $(REPO_PATH) clean

all: $(SOFTFLOAT)

.DEFAULT_GOAL = all

.PHONY: all clean
