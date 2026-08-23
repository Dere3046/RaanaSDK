// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#ifndef RUST_SUPPORT_SHIM_KUNIT_H
#define RUST_SUPPORT_SHIM_KUNIT_H

#include <linux/string.h>
#include <linux/types.h>
#include <linux/version.h>
#include <kunit/test.h>

struct module;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
#include <kunit/test-bug.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 6, 0)
struct kunit_attributes {
	u32 speed;
	unsigned long __kabi_reserved1;
};
#endif

typedef void (*rs_kunit_run_case_t)(struct kunit *);
typedef void (*rs_kunit_generate_params_t)(void);
typedef int (*rs_kunit_suite_suite_init_t)(struct kunit_suite *);
typedef void (*rs_kunit_suite_suite_exit_t)(struct kunit_suite *);
typedef int (*rs_kunit_suite_init_t)(struct kunit *);
typedef void (*rs_kunit_suite_exit_t)(struct kunit *);

rs_kunit_run_case_t rs_kunit_case_run_case(const struct kunit_case *obj);
void rs_kunit_case_set_run_case(struct kunit_case *obj,
				rs_kunit_run_case_t value);
const char *rs_kunit_case_name(const struct kunit_case *obj);
void rs_kunit_case_set_name(struct kunit_case *obj, const char *value);
struct kunit_attributes rs_kunit_case_attr(const struct kunit_case *obj);
void rs_kunit_case_set_attr(struct kunit_case *obj,
			    struct kunit_attributes value);
rs_kunit_generate_params_t rs_kunit_case_generate_params(
	const struct kunit_case *obj);
void rs_kunit_case_set_generate_params(struct kunit_case *obj,
				       rs_kunit_generate_params_t value);
u32 rs_kunit_case_status(const struct kunit_case *obj);
void rs_kunit_case_set_status(struct kunit_case *obj, u32 value);
char *rs_kunit_case_module_name(const struct kunit_case *obj);
void rs_kunit_case_set_module_name(struct kunit_case *obj, char *value);
void *rs_kunit_case_log(const struct kunit_case *obj);
void rs_kunit_case_set_log(struct kunit_case *obj, void *value);
unsigned long rs_kunit_case___kabi_reserved1(const struct kunit_case *obj);
void rs_kunit_case_set___kabi_reserved1(struct kunit_case *obj,
					unsigned long value);

char *rs_kunit_suite_name(struct kunit_suite *obj);
void rs_kunit_suite_set_name(struct kunit_suite *obj, const char *value);
char *rs_kunit_suite_status_comment(struct kunit_suite *obj);
void rs_kunit_suite_set_status_comment(struct kunit_suite *obj,
				       const char *value);
struct kunit_case *rs_kunit_suite_test_cases(const struct kunit_suite *obj);
void rs_kunit_suite_set_test_cases(struct kunit_suite *obj,
				   struct kunit_case *value);
rs_kunit_suite_suite_init_t rs_kunit_suite_suite_init(
	const struct kunit_suite *obj);
void rs_kunit_suite_set_suite_init(struct kunit_suite *obj,
				   rs_kunit_suite_suite_init_t value);
rs_kunit_suite_suite_exit_t rs_kunit_suite_suite_exit(
	const struct kunit_suite *obj);
void rs_kunit_suite_set_suite_exit(struct kunit_suite *obj,
				   rs_kunit_suite_suite_exit_t value);
rs_kunit_suite_init_t rs_kunit_suite_init(const struct kunit_suite *obj);
void rs_kunit_suite_set_init(struct kunit_suite *obj,
			     rs_kunit_suite_init_t value);
rs_kunit_suite_exit_t rs_kunit_suite_exit(const struct kunit_suite *obj);
void rs_kunit_suite_set_exit(struct kunit_suite *obj,
			     rs_kunit_suite_exit_t value);
struct kunit_attributes rs_kunit_suite_attr(const struct kunit_suite *obj);
void rs_kunit_suite_set_attr(struct kunit_suite *obj,
			     struct kunit_attributes value);
void *rs_kunit_suite_debugfs(const struct kunit_suite *obj);
void rs_kunit_suite_set_debugfs(struct kunit_suite *obj, void *value);
void *rs_kunit_suite_log(const struct kunit_suite *obj);
void rs_kunit_suite_set_log(struct kunit_suite *obj, void *value);
int rs_kunit_suite_suite_init_err(const struct kunit_suite *obj);
void rs_kunit_suite_set_suite_init_err(struct kunit_suite *obj, int value);
bool rs_kunit_suite_is_init(const struct kunit_suite *obj);
void rs_kunit_suite_set_is_init(struct kunit_suite *obj, bool value);
unsigned long rs_kunit_suite___kabi_reserved1(const struct kunit_suite *obj);
void rs_kunit_suite_set___kabi_reserved1(struct kunit_suite *obj,
					 unsigned long value);

void rs_kunit_case_array_init(void *cases, unsigned int index,
			      rs_kunit_run_case_t run_case,
			      const char *name);
void rs_kunit_case_array_init_null(void *cases, unsigned int index);
void rs_kunit_suite_init_rust(void *suite, const char *name, void *test_cases);
struct kunit_suite **rs_module_kunit_suites(struct module *mod);
int rs_module_num_kunit_suites(struct module *mod);
void rs_kunit_register_case(struct module *mod, const char *name,
			    rs_kunit_run_case_t run_case);
void rs_kunit_unregister_module(struct module *mod);
void rs_kunit_run_case(struct kunit *test);

extern void *rs_kunit_current_test;

#endif
