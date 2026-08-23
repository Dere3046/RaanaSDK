# SPDX-License-Identifier: GPL-2.0-only

obj-m := rust_support.o

KDIR := $(KDIR)
MDIR := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ODIR := $(MDIR)/out/$(VER)

DEPS := KallRecon Type_info
include $(MDIR)/mk/deps.mk

rust_support-y := src/main.o src/helpers.o src/shim.o \
	src/shim_fs_misc.o src/shim_block.o src/shim_kunit.o src/exports.o \
	rust/core.o rust/compiler_builtins.o rust/rust_support_rust.o \
	$(DEPS_OBJS_ALL)

ccflags-y += -std=gnu11
ccflags-y += -Wno-declaration-after-statement
ccflags-y += -Wno-unused-variable
ccflags-y += -Wno-unused-function
ccflags-y += -Wno-strict-prototypes
ccflags-y += -I$(src)/lib
ccflags-y += $(addprefix -I$(src)/,$(DEPS_INCS_ALL))

$(info -- KDIR: $(KDIR))
$(info -- MDIR: $(MDIR))
$(info -- ODIR: $(ODIR))

all:
	make -C $(KDIR) M=$(ODIR) src=$(MDIR) modules
clean:
	make -C $(KDIR) M=$(ODIR) src=$(MDIR) clean

$(obj)/%.o: $(src)/%.c $(recordmcount_source) FORCE
	$(call if_changed_rule,cc_o_c)
	$(call cmd,force_checksrc)

$(obj)/%.o: $(src)/%.S FORCE
	$(call if_changed_rule,as_o_S)
