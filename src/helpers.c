// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#include <linux/blk-mq.h>
#include <linux/bug.h>
#include <linux/cred.h>
#include <linux/err.h>
#include <linux/errname.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/highmem.h>
#include <linux/jump_label.h>
#include <linux/list_lru.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/mutex.h>
#include <linux/pid_namespace.h>
#include <linux/poll.h>
#include <linux/rbtree.h>
#include <linux/rcupdate.h>
#include <linux/refcount.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/sched/signal.h>
#include <linux/security.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/task_work.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#ifdef CONFIG_KUNIT
#include <kunit/test-bug.h>
#endif

#include "helpers.h"
#include "shim.h"

void rust_helper_BUG(void)
{
	BUG();
}

void *rust_helper_ERR_PTR(long err)
{
	return ERR_PTR(err);
}

bool rust_helper_IS_ERR(const void *ptr)
{
	return IS_ERR(ptr);
}

long rust_helper_PTR_ERR(const void *ptr)
{
	return PTR_ERR(ptr);
}

const char *rust_helper_errname(int err)
{
	return rs_errname(err);
}

struct task_struct *rust_helper_get_current(void)
{
	return current;
}

void rust_helper_get_task_struct(struct task_struct *t)
{
	get_task_struct(t);
}

void rust_helper_put_task_struct(struct task_struct *t)
{
	put_task_struct(t);
}

kuid_t rust_helper_task_uid(struct task_struct *task)
{
	return task_uid(task);
}

kuid_t rust_helper_task_euid(struct task_struct *task)
{
	return task_euid(task);
}

uid_t rust_helper_from_kuid(struct user_namespace *to, kuid_t uid)
{
	return from_kuid(to, uid);
}

bool rust_helper_uid_eq(kuid_t left, kuid_t right)
{
	return uid_eq(left, right);
}

kuid_t rust_helper_current_euid(void)
{
	return current_euid();
}

struct user_namespace *rust_helper_current_user_ns(void)
{
	return current_cred()->user_ns;
}

pid_t rust_helper_task_tgid_nr_ns(struct task_struct *tsk,
				  struct pid_namespace *ns)
{
	return task_tgid_nr_ns(tsk, ns);
}

unsigned long rust_helper_task_rlimit(const struct task_struct *task,
				      unsigned int limit)
{
	return task_rlimit(task, limit);
}

struct pid_namespace *rust_helper_get_pid_ns(struct pid_namespace *ns)
{
	return get_pid_ns(ns);
}

void rust_helper_put_pid_ns(struct pid_namespace *ns)
{
	put_pid_ns(ns);
}

struct pid_namespace *rust_helper_task_get_pid_ns(struct task_struct *task)
{
	struct pid_namespace *pid_ns;

	rcu_read_lock();
	pid_ns = rs_task_active_pid_ns(task);
	if (pid_ns)
		get_pid_ns(pid_ns);
	rcu_read_unlock();
	return pid_ns;
}

struct page *rust_helper_alloc_pages(gfp_t gfp_mask, unsigned int order)
{
	return alloc_pages(gfp_mask, order);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0)
void *rust_helper_kmap_local_page(struct page *page)
{
	return kmap_local_page(page);
}

void rust_helper_kunmap_local(const void *addr)
{
	kunmap_local((void *)addr);
}
#else
void *rust_helper_kmap_local_page(struct page *page)
{
	return kmap_atomic(page);
}

void rust_helper_kunmap_local(const void *addr)
{
	kunmap_atomic((void *)addr);
}
#endif

unsigned long rust_helper_copy_from_user(void *to, const void *from,
					 unsigned long n)
{
	return copy_from_user(to, (const void __user *)from, n);
}

unsigned long rust_helper_copy_to_user(void *to, const void *from,
				       unsigned long n)
{
	return copy_to_user((void __user *)to, from, n);
}

void *rust_helper_krealloc(const void *objp, size_t new_size, gfp_t flags)
{
	return krealloc(objp, new_size, flags);
}

void *rust_helper_kvrealloc(const void *p, size_t size, gfp_t flags)
{
	return rs_kvrealloc(p, size, flags);
}

void *rust_helper_vrealloc(const void *p, size_t size, gfp_t flags)
{
	return rs_vrealloc(p, size, flags);
}

int rust_helper_signal_pending(struct task_struct *t)
{
	return signal_pending(t);
}

void rust_helper_poll_wait(struct file *filp, struct wait_queue_head *wait_address,
			   struct poll_table_struct *p)
{
	poll_wait(filp, wait_address, p);
}

void rust_helper_init_wait(struct wait_queue_entry *wq_entry)
{
	init_wait(wq_entry);
}

void rust_helper___mutex_init(struct mutex *mutex, const char *name,
			      struct lock_class_key *key)
{
	__mutex_init(mutex, name, key);
}

void rust_helper_mutex_lock(struct mutex *lock)
{
	mutex_lock(lock);
}

void rust_helper___spin_lock_init(spinlock_t *lock, const char *name,
				  struct lock_class_key *key)
{
#ifdef CONFIG_DEBUG_SPINLOCK
# if defined(CONFIG_PREEMPT_RT)
	__spin_lock_init(lock, name, key, false);
# else /* !CONFIG_PREEMPT_RT */
	__raw_spin_lock_init(spinlock_check(lock), name, key, LD_WAIT_CONFIG);
# endif /* CONFIG_PREEMPT_RT */
#else /* !CONFIG_DEBUG_SPINLOCK */
	spin_lock_init(lock);
#endif /* CONFIG_DEBUG_SPINLOCK */
}

void rust_helper_spin_lock(spinlock_t *lock)
{
	spin_lock(lock);
}

void rust_helper_spin_unlock(spinlock_t *lock)
{
	spin_unlock(lock);
}

int rust_helper_spin_trylock(spinlock_t *lock)
{
	return spin_trylock(lock);
}

refcount_t rust_helper_REFCOUNT_INIT(int n)
{
	return (refcount_t)REFCOUNT_INIT(n);
}

void rust_helper_refcount_inc(refcount_t *r)
{
	refcount_inc(r);
}

bool rust_helper_refcount_dec_and_test(refcount_t *r)
{
	return refcount_dec_and_test(r);
}

const struct cred *rust_helper_get_cred(const struct cred *cred)
{
	return get_cred(cred);
}

void rust_helper_put_cred(const struct cred *cred)
{
	put_cred(cred);
}

struct file *rust_helper_get_file(struct file *f)
{
	return get_file(f);
}

loff_t rust_helper_i_size_read(const struct inode *inode)
{
	return i_size_read(inode);
}

/* Helpers copied from upstream 6.12 rust/helpers to complete the set. */

void *rust_helper_blk_mq_rq_to_pdu(struct request *rq)
{
	return blk_mq_rq_to_pdu(rq);
}

struct request *rust_helper_blk_mq_rq_from_pdu(void *pdu)
{
	return blk_mq_rq_from_pdu(pdu);
}

unsigned long rust_helper_list_lru_count(struct list_lru *lru)
{
	return rs_list_lru_count(lru);
}

unsigned long rust_helper_list_lru_walk(struct list_lru *lru,
					list_lru_walk_cb isolate, void *cb_arg,
					unsigned long nr_to_walk)
{
	return rs_list_lru_walk(lru, isolate, cb_arg, nr_to_walk);
}

void rust_helper_init_task_work(struct callback_head *twork,
				task_work_func_t func)
{
	init_task_work(twork, func);
}

void rust_helper_init_work_with_key(struct work_struct *work, work_func_t func,
				    bool onstack, const char *name,
				    struct lock_class_key *key)
{
	__init_work(work, onstack);
	work->data = (atomic_long_t)WORK_DATA_INIT();
	lockdep_init_map(&work->lockdep_map, name, key, 0);
	INIT_LIST_HEAD(&work->entry);
	work->func = func;
}

struct kunit *rust_helper_kunit_get_current_test(void)
{
#if defined(CONFIG_KUNIT) && LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	return kunit_get_current_test();
#else
	extern void *rs_kunit_current_test;

	return (struct kunit *)rs_kunit_current_test;
#endif
}

void rust_helper_lockdep_set_class_rwsem(struct rw_semaphore *lock,
					 struct lock_class_key *key,
					 const char *name)
{
	lockdep_set_class_and_name(lock, key, name);
}

void rust_helper_mmgrab(struct mm_struct *mm)
{
	mmgrab(mm);
}

void rust_helper_mmdrop(struct mm_struct *mm)
{
	mmdrop(mm);
}

void rust_helper_mmget(struct mm_struct *mm)
{
	mmget(mm);
}

bool rust_helper_mmget_not_zero(struct mm_struct *mm)
{
	return mmget_not_zero(mm);
}

void rust_helper_mmap_read_lock(struct mm_struct *mm)
{
	mmap_read_lock(mm);
}

bool rust_helper_mmap_read_trylock(struct mm_struct *mm)
{
	return mmap_read_trylock(mm);
}

void rust_helper_mmap_read_unlock(struct mm_struct *mm)
{
	mmap_read_unlock(mm);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
struct vm_area_struct *rust_helper_vma_lookup(struct mm_struct *mm,
					      unsigned long addr)
{
	return vma_lookup(mm, addr);
}

void rust_helper_vma_end_read(struct vm_area_struct *vma)
{
	vma_end_read(vma);
}
#endif

void rust_helper_rb_link_node(struct rb_node *node, struct rb_node *parent,
			      struct rb_node **rb_link)
{
	rb_link_node(node, parent, rb_link);
}

#ifndef CONFIG_SECURITY
void rust_helper_security_cred_getsecid(const struct cred *c, u32 *secid)
{
	security_cred_getsecid(c, secid);
}

int rust_helper_security_secid_to_secctx(u32 secid, char **secdata, u32 *seclen)
{
	return security_secid_to_secctx(secid, secdata, seclen);
}

void rust_helper_security_release_secctx(char *secdata, u32 seclen)
{
	security_release_secctx(secdata, seclen);
}

int rust_helper_security_binder_set_context_mgr(const struct cred *mgr)
{
	return security_binder_set_context_mgr(mgr);
}

int rust_helper_security_binder_transaction(const struct cred *from,
					    const struct cred *to)
{
	return security_binder_transaction(from, to);
}

int rust_helper_security_binder_transfer_binder(const struct cred *from,
						const struct cred *to)
{
	return security_binder_transfer_binder(from, to);
}

int rust_helper_security_binder_transfer_file(const struct cred *from,
					      const struct cred *to,
					      const struct file *file)
{
	return security_binder_transfer_file(from, to, file);
}
#endif

#ifndef CONFIG_JUMP_LABEL
int rust_helper_static_key_count(struct static_key *key)
{
	return static_key_count(key);
}
#endif
