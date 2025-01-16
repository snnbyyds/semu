# Copyright 2025 Nuo Shen, Nanjing University
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

SRCS-y += src/semu-main.c
DIRS-y += src/cpu src/memory src/monitor src/utils src/device
LIBS += -lreadline -lSDL3 -lSDL3_image -lpthread -ldl -lm

SHARE = $(if $(CONFIG_TARGET_SHARE),1,0)

LIBSOFTFLOAT = libs/softfloat/repo/build/Linux-x86_64-GCC/softfloat.a
CFLAGS += -I libs/softfloat/repo/source/include

$(LIBSOFTFLOAT):
	$(MAKE) -C libs/softfloat
