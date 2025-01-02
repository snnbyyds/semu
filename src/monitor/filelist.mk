ifeq ($(CONFIG_USE_GDBSTUB),)
SRCS-BLACKLIST-y += src/monitor/gdbstub/gdbstub.c
else
LIBGDBSTUB = tools/gdbstub/repo/build/libgdbstub.a
CFLAGS += -I tools/gdbstub/repo/include
src/monitor/gdbstub/gdbstub.c: $(LIBGDBSTUB)
$(LIBGDBSTUB):
	$(MAKE) -C tools/gdbstub
endif
