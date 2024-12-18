ifneq ($(wildcard repo/include/gdbstub.h),)
	DIRS-BLACKLIST-y += src/monitor/gdbstub/repo/tests src/monitor/gdbstub/repo
endif

ifeq ($(CONFIG_USE_GDBSTUB),)
SRCS-BLACKLIST-y += src/monitor/gdbstub/gdbstub.c
else
LIBGDBSTUB = src/monitor/gdbstub/repo/build/libgdbstub.a
CFLAGS += -I src/monitor/gdbstub/repo/include -Wno-unused-variable
src/monitor/gdbstub/gdbstub.c: $(LIBGDBSTUB)
$(LIBGDBSTUB):
	$(MAKE) -C src/monitor/gdbstub
endif
