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

include $(SEMU_HOME)/scripts/build.mk
include $(SEMU_HOME)/tools/difftest.mk

override ARGS ?=

IMG ?=
SEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(SEMU_EXEC)

gdb: run-env
	gdb -s $(BINARY) --args $(SEMU_EXEC)

clean-all: clean distclean

.PHONY: run gdb run-env clean-all
