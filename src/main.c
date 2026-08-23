// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/printk.h>

#include "core.h"
#include "type_info.h"
#include "shim.h"

extern void rs_kunit_init_rust(struct module *mod);
extern void rs_kunit_unregister_module(struct module *mod);

static int rs_kunit_module_notify(struct notifier_block *nb, unsigned long val,
				  void *data)
{
	struct module *mod = data;

	(void)nb;
	if (val == MODULE_STATE_COMING) {
		rs_kunit_init_rust(mod);
	} else if (val == MODULE_STATE_GOING) {
		rs_kunit_unregister_module(mod);
	}

	return 0;
}

static struct notifier_block rs_kunit_nb = {
	.notifier_call = rs_kunit_module_notify,
	.priority = 1,
};

static struct ti_resolver rs_res = {
	.name_to_addr = rs_resolve_name,
};

static int __init rust_support_init(void)
{
	int ret;

	find_kallsyms_base();
	if (!klnum_val || !kallrecon_klp) {
		pr_err("[rust_support] kallsyms recovery failed\n");
		return -ENODATA;
	}

	ret = ti_init(&rs_res);
	if (ret) {
		pr_err("[rust_support] type_info init failed: %d\n", ret);
		return ret;
	}

	ret = rs_shim_init();
	if (ret) {
		ti_exit();
		return ret;
	}

	rs_kunit_init_rust(&__this_module);
	register_module_notifier(&rs_kunit_nb);

	pr_info("[rust_support] loaded syms=%u btf=%d\n",
		klnum_val, ti_btf_available() ? 1 : 0);
	return 0;
}

static void __exit rust_support_exit(void)
{
	unregister_module_notifier(&rs_kunit_nb);
	rs_shim_exit();
	ti_exit();
	pr_info("[rust_support] unloaded\n");
}

module_init(rust_support_init);
module_exit(rust_support_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Rust support LKM for old GKI kernels");
MODULE_AUTHOR("dere3046");
