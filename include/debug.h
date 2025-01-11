/*
 * Copyright 2025 Nuo Shen, Nanjing University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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