SRCS-y += src/semu-main.c
DIRS-y += src/cpu src/memory src/monitor src/utils src/device
LIBS += -lreadline -lSDL3 -lSDL3_image -lpthread -ldl -lm

LIBSOFTFLOAT = libs/softfloat/repo/build/Linux-x86_64-GCC/softfloat.a
CFLAGS += -I libs/softfloat/repo/source/include

$(LIBSOFTFLOAT):
	$(MAKE) -C libs/softfloat
