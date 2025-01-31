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

ifeq ($(CONFIG_ENABLE_ITRACE),)
SRCS-BLACKLIST-y += src/utils/disasm.c
else
LIBCAPSTONE = tools/capstone/repo/libcapstone.so.6
CFLAGS += -I tools/capstone/repo/include -DLIBCAPSTONE=\"$(LIBCAPSTONE)\"

src/utils/disasm.c: $(LIBCAPSTONE)
$(LIBCAPSTONE):
	$(MAKE) -C tools/capstone
endif
