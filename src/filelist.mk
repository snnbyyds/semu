SRCS-y += src/semu-main.c
DIRS-y += src/cpu src/memory src/monitor src/utils src/device
LIBS += -lreadline -lSDL3 -lpthread
