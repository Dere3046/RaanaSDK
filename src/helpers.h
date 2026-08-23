// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#ifndef RUST_SUPPORT_HELPERS_H
#define RUST_SUPPORT_HELPERS_H

#include <linux/types.h>
#include <linux/version.h>
#include <linux/gfp.h>
#include <linux/uidgid.h>
#include <linux/refcount.h>
#include <linux/spinlock_types.h>
#include <linux/mutex.h>
#include <linux/blk-mq.h>
#include <linux/list_lru.h>
#include <linux/rbtree.h>
#include <linux/rwsem.h>
#include <linux/task_work.h>
#include <linux/workqueue.h>
#ifdef CONFIG_KUNIT
#include <kunit/test-bug.h>
#endif

struct task_struct;
struct user_namespace;
struct pid_namespace;
struct page;
struct file;
struct wait_queue_head;
struct poll_table_struct;
struct wait_queue_entry;
struct cred;
struct inode;
struct lock_class_key;
struct mm_struct;
struct vm_area_struct;
struct static_key;

void rust_helper_BUG(void);
void *rust_helper_ERR_PTR(long err);
bool rust_helper_IS_ERR(const void *ptr);
long rust_helper_PTR_ERR(const void *ptr);
const char *rust_helper_errname(int err);

struct task_struct *rust_helper_get_current(void);
void rust_helper_get_task_struct(struct task_struct *t);
void rust_helper_put_task_struct(struct task_struct *t);
kuid_t rust_helper_task_uid(struct task_struct *task);
kuid_t rust_helper_task_euid(struct task_struct *task);
uid_t rust_helper_from_kuid(struct user_namespace *to, kuid_t uid);
bool rust_helper_uid_eq(kuid_t left, kuid_t right);
kuid_t rust_helper_current_euid(void);
struct user_namespace *rust_helper_current_user_ns(void);
pid_t rust_helper_task_tgid_nr_ns(struct task_struct *tsk,
				  struct pid_namespace *ns);
unsigned long rust_helper_task_rlimit(const struct task_struct *task,
				      unsigned int limit);
struct pid_namespace *rust_helper_get_pid_ns(struct pid_namespace *ns);
void rust_helper_put_pid_ns(struct pid_namespace *ns);
struct pid_namespace *rust_helper_task_get_pid_ns(struct task_struct *task);

struct page *rust_helper_alloc_pages(gfp_t gfp_mask, unsigned int order);
void *rust_helper_kmap_local_page(struct page *page);
void rust_helper_kunmap_local(const void *addr);

unsigned long rust_helper_copy_from_user(void *to, const void *from,
					 unsigned long n);
unsigned long rust_helper_copy_to_user(void *to, const void *from,
				       unsigned long n);

void *rust_helper_krealloc(const void *objp, size_t new_size, gfp_t flags);
void *rust_helper_kvrealloc(const void *p, size_t size, gfp_t flags);
void *rust_helper_vrealloc(const void *p, size_t size, gfp_t flags);

int rust_helper_signal_pending(struct task_struct *t);
void rust_helper_poll_wait(struct file *filp, struct wait_queue_head *wait_address,
			   struct poll_table_struct *p);
void rust_helper_init_wait(struct wait_queue_entry *wq_entry);

void rust_helper___mutex_init(struct mutex *mutex, const char *name,
			      struct lock_class_key *key);
void rust_helper_mutex_lock(struct mutex *lock);
void rust_helper___spin_lock_init(spinlock_t *lock, const char *name,
				  struct lock_class_key *key);
void rust_helper_spin_lock(spinlock_t *lock);
void rust_helper_spin_unlock(spinlock_t *lock);
int rust_helper_spin_trylock(spinlock_t *lock);

refcount_t rust_helper_REFCOUNT_INIT(int n);
void rust_helper_refcount_inc(refcount_t *r);
bool rust_helper_refcount_dec_and_test(refcount_t *r);

const struct cred *rust_helper_get_cred(const struct cred *cred);
void rust_helper_put_cred(const struct cred *cred);

struct file *rust_helper_get_file(struct file *f);
loff_t rust_helper_i_size_read(const struct inode *inode);

/* Helpers copied from upstream 6.12 rust/helpers to complete the set. */
void *rust_helper_blk_mq_rq_to_pdu(struct request *rq);
struct request *rust_helper_blk_mq_rq_from_pdu(void *pdu);
unsigned long rust_helper_list_lru_count(struct list_lru *lru);
unsigned long rust_helper_list_lru_walk(struct list_lru *lru,
					list_lru_walk_cb isolate, void *cb_arg,
					unsigned long nr_to_walk);
void rust_helper_init_task_work(struct callback_head *twork,
				task_work_func_t func);
void rust_helper_init_work_with_key(struct work_struct *work, work_func_t func,
				    bool onstack, const char *name,
				    struct lock_class_key *key);
struct kunit *rust_helper_kunit_get_current_test(void);
void rust_helper_lockdep_set_class_rwsem(struct rw_semaphore *lock,
					 struct lock_class_key *key,
					 const char *name);
void rust_helper_mmgrab(struct mm_struct *mm);
void rust_helper_mmdrop(struct mm_struct *mm);
void rust_helper_mmget(struct mm_struct *mm);
bool rust_helper_mmget_not_zero(struct mm_struct *mm);
void rust_helper_mmap_read_lock(struct mm_struct *mm);
bool rust_helper_mmap_read_trylock(struct mm_struct *mm);
void rust_helper_mmap_read_unlock(struct mm_struct *mm);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
struct vm_area_struct *rust_helper_vma_lookup(struct mm_struct *mm,
					      unsigned long addr);
void rust_helper_vma_end_read(struct vm_area_struct *vma);
#endif
void rust_helper_rb_link_node(struct rb_node *node, struct rb_node *parent,
			      struct rb_node **rb_link);
#ifndef CONFIG_SECURITY
void rust_helper_security_cred_getsecid(const struct cred *c, u32 *secid);
int rust_helper_security_secid_to_secctx(u32 secid, char **secdata, u32 *seclen);
void rust_helper_security_release_secctx(char *secdata, u32 seclen);
int rust_helper_security_binder_set_context_mgr(const struct cred *mgr);
int rust_helper_security_binder_transaction(const struct cred *from,
					    const struct cred *to);
int rust_helper_security_binder_transfer_binder(const struct cred *from,
						const struct cred *to);
int rust_helper_security_binder_transfer_file(const struct cred *from,
					      const struct cred *to,
					      const struct file *file);
#endif
#ifndef CONFIG_JUMP_LABEL
int rust_helper_static_key_count(struct static_key *key);
#endif

#endif
