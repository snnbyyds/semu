ifeq ($(CONFIG_ENABLE_ITRACE),)
SRCS-BLACKLIST-y += src/utils/disasm.c
else
LIBCAPSTONE = tools/capstone/repo/libcapstone.so.6
CFLAGS += -I tools/capstone/repo/include -DLIBCAPSTONE=\"$(LIBCAPSTONE)\"

src/utils/disasm.c: $(LIBCAPSTONE)
$(LIBCAPSTONE):
	$(MAKE) -C tools/capstone
endif
