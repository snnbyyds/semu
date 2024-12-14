#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <common.h>

#define KEYDOWN_MASK 0x8000
#define KEYDOWN_SHIFT 15

void init_device();
void update_device();

#endif
