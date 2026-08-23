// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#include <linux/export.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/version.h>

#include "shim_kunit.h"

#define RS_KUNIT_MAX_TEST_CASES 256

struct rs_kunit_test_entry {
	struct module *owner;
	const char *name;
	rs_kunit_run_case_t run_case;
};

static struct rs_kunit_test_entry rs_kunit_test_entries[RS_KUNIT_MAX_TEST_CASES];
static int rs_kunit_test_count;
static DEFINE_SPINLOCK(rs_kunit_test_lock);

void *rs_kunit_current_test;

__nocfi rs_kunit_run_case_t rs_kunit_case_run_case(
	const struct kunit_case *obj)
{
	return obj->run_case;
}

__nocfi void rs_kunit_case_set_run_case(struct kunit_case *obj,
					rs_kunit_run_case_t value)
{
	obj->run_case = value;
}

__nocfi const char *rs_kunit_case_name(const struct kunit_case *obj)
{
	return obj->name;
}

__nocfi void rs_kunit_case_set_name(struct kunit_case *obj, const char *value)
{
	obj->name = value;
}

__nocfi struct kunit_attributes rs_kunit_case_attr(
	const struct kunit_case *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	return obj->attr;
#else
	struct kunit_attributes attr = { 0 };

	return attr;
#endif
}

__nocfi void rs_kunit_case_set_attr(struct kunit_case *obj,
				    struct kunit_attributes value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	obj->attr = value;
#endif
}

__nocfi rs_kunit_generate_params_t rs_kunit_case_generate_params(
	const struct kunit_case *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	union {
		const void *(*kernel_fn)(const void *, char *);
		rs_kunit_generate_params_t rust_fn;
	} u;

	u.kernel_fn = obj->generate_params;
	return u.rust_fn;
#else
	return NULL;
#endif
}

__nocfi void rs_kunit_case_set_generate_params(struct kunit_case *obj,
					       rs_kunit_generate_params_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	union {
		rs_kunit_generate_params_t rust_fn;
		const void *(*kernel_fn)(const void *, char *);
	} u;

	u.rust_fn = value;
	obj->generate_params = u.kernel_fn;
#endif
}

__nocfi u32 rs_kunit_case_status(const struct kunit_case *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	return (u32)obj->status;
#else
	return obj->success ? 0U : 1U;
#endif
}

__nocfi void rs_kunit_case_set_status(struct kunit_case *obj, u32 value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	obj->status = (enum kunit_status)value;
#else
	obj->success = (value == 0U);
#endif
}

__nocfi char *rs_kunit_case_module_name(const struct kunit_case *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	return obj->module_name;
#else
	return NULL;
#endif
}

__nocfi void rs_kunit_case_set_module_name(struct kunit_case *obj,
					   char *value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	obj->module_name = value;
#endif
}

__nocfi void *rs_kunit_case_log(const struct kunit_case *obj)
{
	return (void *)obj->log;
}

__nocfi void rs_kunit_case_set_log(struct kunit_case *obj, void *value)
{
	obj->log = value;
}

__nocfi unsigned long rs_kunit_case___kabi_reserved1(
	const struct kunit_case *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	return (unsigned long)obj->__kabi_reserved1;
#else
	return 0;
#endif
}

__nocfi void rs_kunit_case_set___kabi_reserved1(struct kunit_case *obj,
						unsigned long value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	obj->__kabi_reserved1 = value;
#endif
}

__nocfi char *rs_kunit_suite_name(struct kunit_suite *obj)
{
	return (char *)obj->name;
}

__nocfi void rs_kunit_suite_set_name(struct kunit_suite *obj,
				     const char *value)
{
	if (value)
		strscpy((char *)obj->name, value, sizeof(obj->name));
}

__nocfi char *rs_kunit_suite_status_comment(struct kunit_suite *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	return obj->status_comment;
#else
	return NULL;
#endif
}

__nocfi void rs_kunit_suite_set_status_comment(struct kunit_suite *obj,
					       const char *value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	if (value)
		strscpy(obj->status_comment, value,
			sizeof(obj->status_comment));
#endif
}

__nocfi struct kunit_case *rs_kunit_suite_test_cases(
	const struct kunit_suite *obj)
{
	return obj->test_cases;
}

__nocfi void rs_kunit_suite_set_test_cases(struct kunit_suite *obj,
					   struct kunit_case *value)
{
	obj->test_cases = value;
}

__nocfi rs_kunit_suite_suite_init_t rs_kunit_suite_suite_init(
	const struct kunit_suite *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	return obj->suite_init;
#else
	return NULL;
#endif
}

__nocfi void rs_kunit_suite_set_suite_init(struct kunit_suite *obj,
					   rs_kunit_suite_suite_init_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	obj->suite_init = value;
#endif
}

__nocfi rs_kunit_suite_suite_exit_t rs_kunit_suite_suite_exit(
	const struct kunit_suite *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	return obj->suite_exit;
#else
	return NULL;
#endif
}

__nocfi void rs_kunit_suite_set_suite_exit(struct kunit_suite *obj,
					   rs_kunit_suite_suite_exit_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	obj->suite_exit = value;
#endif
}

__nocfi rs_kunit_suite_init_t rs_kunit_suite_init(
	const struct kunit_suite *obj)
{
	return obj->init;
}

__nocfi void rs_kunit_suite_set_init(struct kunit_suite *obj,
				     rs_kunit_suite_init_t value)
{
	obj->init = value;
}

__nocfi rs_kunit_suite_exit_t rs_kunit_suite_exit(
	const struct kunit_suite *obj)
{
	return obj->exit;
}

__nocfi void rs_kunit_suite_set_exit(struct kunit_suite *obj,
				     rs_kunit_suite_exit_t value)
{
	obj->exit = value;
}

__nocfi struct kunit_attributes rs_kunit_suite_attr(
	const struct kunit_suite *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	return obj->attr;
#else
	struct kunit_attributes attr = { 0 };

	return attr;
#endif
}

__nocfi void rs_kunit_suite_set_attr(struct kunit_suite *obj,
				     struct kunit_attributes value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	obj->attr = value;
#endif
}

__nocfi void *rs_kunit_suite_debugfs(const struct kunit_suite *obj)
{
	return (void *)obj->debugfs;
}

__nocfi void rs_kunit_suite_set_debugfs(struct kunit_suite *obj, void *value)
{
	obj->debugfs = value;
}

__nocfi void *rs_kunit_suite_log(const struct kunit_suite *obj)
{
	return (void *)obj->log;
}

__nocfi void rs_kunit_suite_set_log(struct kunit_suite *obj, void *value)
{
	obj->log = value;
}

__nocfi int rs_kunit_suite_suite_init_err(const struct kunit_suite *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	return obj->suite_init_err;
#else
	return 0;
#endif
}

__nocfi void rs_kunit_suite_set_suite_init_err(struct kunit_suite *obj,
					       int value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	obj->suite_init_err = value;
#endif
}

__nocfi bool rs_kunit_suite_is_init(const struct kunit_suite *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	return obj->is_init;
#else
	return false;
#endif
}

__nocfi void rs_kunit_suite_set_is_init(struct kunit_suite *obj, bool value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	obj->is_init = value;
#endif
}

__nocfi unsigned long rs_kunit_suite___kabi_reserved1(
	const struct kunit_suite *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	return (unsigned long)obj->__kabi_reserved1;
#else
	return 0;
#endif
}

__nocfi void rs_kunit_suite_set___kabi_reserved1(struct kunit_suite *obj,
						 unsigned long value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	obj->__kabi_reserved1 = value;
#endif
}

__nocfi void rs_kunit_case_array_init(void *cases, unsigned int index,
				      rs_kunit_run_case_t run_case,
				      const char *name)
{
	struct kunit_case *obj = &((struct kunit_case *)cases)[index];

	/*
	 * KUnit calls `run_case` indirectly, so it must be a Clang-compiled
	 * function with a KCFI-compatible type. The actual Rust test function
	 * is registered in `rs_kunit_test_entries` and invoked from the C
	 * dispatcher below with a direct call.
	 */
	(void)run_case;
	memset(obj, 0, sizeof(*obj));
	obj->run_case = rs_kunit_run_case;
	obj->name = name;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	obj->status = KUNIT_SUCCESS;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	obj->attr.speed = KUNIT_SPEED_NORMAL;
	obj->module_name = NULL;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	obj->__kabi_reserved1 = 0;
#endif
}

__nocfi void rs_kunit_case_array_init_null(void *cases, unsigned int index)
{
	memset(&((struct kunit_case *)cases)[index], 0,
	       sizeof(struct kunit_case));
}

__nocfi void rs_kunit_suite_init_rust(void *suite, const char *name,
				      void *test_cases)
{
	struct kunit_suite *s = suite;

	memset(s, 0, sizeof(*s));
	if (name) {
		strscpy((char *)s->name, name, sizeof(s->name));
	}
	s->test_cases = test_cases;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	s->attr.speed = KUNIT_SPEED_NORMAL;
#endif
}

__nocfi void rs_kunit_register_case(struct module *mod, const char *name,
				    rs_kunit_run_case_t run_case)
{
	unsigned long flags;

	if (!name || !run_case) {
		return;
	}

	spin_lock_irqsave(&rs_kunit_test_lock, flags);
	if (rs_kunit_test_count >= RS_KUNIT_MAX_TEST_CASES) {
		spin_unlock_irqrestore(&rs_kunit_test_lock, flags);
		pr_warn("rust_kunit: test case registry full, dropping %s\n",
			name);
		return;
	}
	rs_kunit_test_entries[rs_kunit_test_count].owner = mod;
	rs_kunit_test_entries[rs_kunit_test_count].name = name;
	rs_kunit_test_entries[rs_kunit_test_count].run_case = run_case;
	rs_kunit_test_count++;
	spin_unlock_irqrestore(&rs_kunit_test_lock, flags);
}

__nocfi void rs_kunit_unregister_module(struct module *mod)
{
	unsigned long flags;
	int i;
	int j;

	spin_lock_irqsave(&rs_kunit_test_lock, flags);
	j = 0;
	for (i = 0; i < rs_kunit_test_count; i++) {
		if (rs_kunit_test_entries[i].owner == mod) {
			continue;
		}
		if (j != i) {
			rs_kunit_test_entries[j] = rs_kunit_test_entries[i];
		}
		j++;
	}
	rs_kunit_test_count = j;
	spin_unlock_irqrestore(&rs_kunit_test_lock, flags);
}

__nocfi void rs_kunit_run_case(struct kunit *test)
{
	const char *name = test->name;
	unsigned long flags;
	rs_kunit_run_case_t run_case = NULL;
	int i;

	spin_lock_irqsave(&rs_kunit_test_lock, flags);
	for (i = 0; i < rs_kunit_test_count; i++) {
		if (strcmp(rs_kunit_test_entries[i].name, name) == 0) {
			run_case = rs_kunit_test_entries[i].run_case;
			break;
		}
	}
	spin_unlock_irqrestore(&rs_kunit_test_lock, flags);

	if (!run_case) {
		pr_err("rust_kunit: no Rust test registered for %s\n", name);
		return;
	}

	/* Direct call: no KCFI check on the Rust function. */
	rs_kunit_current_test = test;
	run_case(test);
	rs_kunit_current_test = NULL;
}

__nocfi struct kunit_suite **rs_module_kunit_suites(struct module *mod)
{
#ifdef CONFIG_KUNIT
	return mod->kunit_suites;
#else
	return NULL;
#endif
}

__nocfi int rs_module_num_kunit_suites(struct module *mod)
{
#ifdef CONFIG_KUNIT
	return mod->num_kunit_suites;
#else
	return 0;
#endif
}

EXPORT_SYMBOL_GPL(rs_kunit_case_run_case);
EXPORT_SYMBOL_GPL(rs_kunit_case_set_run_case);
EXPORT_SYMBOL_GPL(rs_kunit_case_name);
EXPORT_SYMBOL_GPL(rs_kunit_case_set_name);
EXPORT_SYMBOL_GPL(rs_kunit_case_attr);
EXPORT_SYMBOL_GPL(rs_kunit_case_set_attr);
EXPORT_SYMBOL_GPL(rs_kunit_case_generate_params);
EXPORT_SYMBOL_GPL(rs_kunit_case_set_generate_params);
EXPORT_SYMBOL_GPL(rs_kunit_case_status);
EXPORT_SYMBOL_GPL(rs_kunit_case_set_status);
EXPORT_SYMBOL_GPL(rs_kunit_case_module_name);
EXPORT_SYMBOL_GPL(rs_kunit_case_set_module_name);
EXPORT_SYMBOL_GPL(rs_kunit_case_log);
EXPORT_SYMBOL_GPL(rs_kunit_case_set_log);
EXPORT_SYMBOL_GPL(rs_kunit_case___kabi_reserved1);
EXPORT_SYMBOL_GPL(rs_kunit_case_set___kabi_reserved1);
EXPORT_SYMBOL_GPL(rs_kunit_suite_name);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_name);
EXPORT_SYMBOL_GPL(rs_kunit_suite_status_comment);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_status_comment);
EXPORT_SYMBOL_GPL(rs_kunit_suite_test_cases);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_test_cases);
EXPORT_SYMBOL_GPL(rs_kunit_suite_suite_init);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_suite_init);
EXPORT_SYMBOL_GPL(rs_kunit_suite_suite_exit);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_suite_exit);
EXPORT_SYMBOL_GPL(rs_kunit_suite_init);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_init);
EXPORT_SYMBOL_GPL(rs_kunit_suite_exit);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_exit);
EXPORT_SYMBOL_GPL(rs_kunit_suite_attr);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_attr);
EXPORT_SYMBOL_GPL(rs_kunit_suite_is_init);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_is_init);
EXPORT_SYMBOL_GPL(rs_kunit_suite_debugfs);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_debugfs);
EXPORT_SYMBOL_GPL(rs_kunit_suite_log);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_log);
EXPORT_SYMBOL_GPL(rs_kunit_suite___kabi_reserved1);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set___kabi_reserved1);
EXPORT_SYMBOL_GPL(rs_kunit_suite_suite_init_err);
EXPORT_SYMBOL_GPL(rs_kunit_suite_set_suite_init_err);
EXPORT_SYMBOL_GPL(rs_kunit_case_array_init);
EXPORT_SYMBOL_GPL(rs_kunit_case_array_init_null);
EXPORT_SYMBOL_GPL(rs_kunit_suite_init_rust);
EXPORT_SYMBOL_GPL(rs_module_kunit_suites);
EXPORT_SYMBOL_GPL(rs_module_num_kunit_suites);
EXPORT_SYMBOL_GPL(rs_kunit_register_case);
EXPORT_SYMBOL_GPL(rs_kunit_unregister_module);
EXPORT_SYMBOL_GPL(rs_kunit_run_case);
