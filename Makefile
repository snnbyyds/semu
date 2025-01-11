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

NAME = semu
remove_quote = $(patsubst "%",%,$(1))

# Sanity check
ifeq ($(wildcard $(SEMU_HOME)/src/semu-main.c),)
  $(error Run ". ./scripts/envsetup.sh" first)
endif

# Include variables and rules generated by menuconfig
-include $(SEMU_HOME)/include/config/auto.conf
-include $(SEMU_HOME)/include/config/auto.conf.cmd

# Include all filelist.mk to merge file lists
FILELIST_MK = $(shell find -L ./src -name "filelist.mk")
include $(FILELIST_MK)

# Filter out directories and files in blacklist to obtain the final set of source files
DIRS-BLACKLIST-y += $(DIRS-BLACKLIST)
SRCS-BLACKLIST-y += $(SRCS-BLACKLIST) $(shell find -L $(DIRS-BLACKLIST-y) -name "*.c")
SRCS-y += $(shell find -L $(DIRS-y) -name "*.c")
SRCS = $(filter-out $(SRCS-BLACKLIST-y),$(SRCS-y))

# Extract compiler and options from menuconfig
ifneq ($(CONFIG_CC),)
CC = $(call remove_quote,$(CONFIG_CC))
endif
CFLAGS_BUILD += $(call remove_quote,$(CONFIG_CC_OPT))
CFLAGS_BUILD += $(if $(CONFIG_CC_LTO),-flto,)
CFLAGS_BUILD += $(if $(CONFIG_CC_DEBUG),-Og -ggdb3,)
CFLAGS_BUILD += $(if $(CONFIG_CC_ASAN),-fsanitize=address,)
CFLAGS  += $(CFLAGS_BUILD) -DVERSION="2.0-CPL2024-preview"
LDFLAGS += $(CFLAGS_BUILD)

# Include rules for menuconfig
include $(SEMU_HOME)/scripts/config.mk

# Include rules to build SEMU
include $(SEMU_HOME)/scripts/native.mk
