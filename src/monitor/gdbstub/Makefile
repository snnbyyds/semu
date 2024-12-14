REPO_PATH = repo
ifeq ($(wildcard repo/include/gdbstub.h),)
  $(shell git clone --depth=2 -b main https://github.com/RinHizakura/mini-gdbstub.git $(REPO_PATH))
endif

GDBSTUB = $(REPO_PATH)/build/libgdbstub.a
$(GDBSTUB):
	cd $(REPO_PATH) && make

clean:
	$(MAKE) -C $(REPO_PATH) clean

all: $(GDBSTUB)
.DEFAULT_GOAL = all

.PHONY: all clean
