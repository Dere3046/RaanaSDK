// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>

#include "core.h"

static int __init mymod_init(void)
{
	unsigned long addr;

	find_kallsyms_base();
	if (!klnum_val || !kallrecon_klp) {
		pr_info("[mymod] kallsyms recovery failed\n");
		return -ENODATA;
	}

	addr = kallrecon_klp("_printk");
	pr_info("[mymod] loaded syms=%u _printk=0x%lx\n", klnum_val, addr);
	return 0;
}

static void __exit mymod_exit(void)
{
	pr_info("[mymod] unloaded\n");
}

module_init(mymod_init);
module_exit(mymod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LKM template module");
MODULE_AUTHOR("dere3046");
