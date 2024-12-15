#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

#define Log(format, ...) \
  printf("\33[1;97m[%s,%d,%s] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Info(format, ...) \
  printf("\33[1;32m[INFO %s:%d %s] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define Warn(format, ...) \
  printf("\33[1;33m[WARN %s:%d %s] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define Error(format, ...) \
  printf("\33[1;31m[ERROR %s:%d %s] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#ifdef CONFIG_RUNTIME_CHECK
#define Assert(cond) assert(cond)
#else
#define Assert(cond)
#endif

#endif