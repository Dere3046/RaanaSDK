// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#ifndef RUST_SUPPORT_SHIM_H
#define RUST_SUPPORT_SHIM_H

#include <linux/types.h>
#include <linux/blkdev.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/jump_label.h>
#include <linux/list_lru.h>
#include <linux/miscdevice.h>
#include <linux/mm_types.h>
#include <linux/rbtree.h>
#include <linux/uidgid.h>
#include <linux/uio.h>
#include <linux/workqueue.h>

struct task_struct;
struct mm_struct;
struct cred;
struct pid_namespace;
struct page;
struct seq_file;
struct module;
struct wait_queue_head;
struct wait_queue_entry;
struct file;
struct poll_table_struct;
struct mutex;
struct sched_param;
struct lock_class_key;
struct vm_struct;

int rs_shim_init(void);
void rs_shim_exit(void);
unsigned long rs_resolve_name(const char *name);

const char *rs_errname(int err);
void *rs_kvrealloc(const void *p, size_t size, unsigned int flags);
void *rs_vrealloc(const void *p, size_t size, unsigned int flags);
struct vm_struct *rs_find_vm_area(const void *addr);
unsigned long rs_list_lru_count(struct list_lru *lru);
unsigned long rs_list_lru_walk(struct list_lru *lru,
			       list_lru_walk_cb isolate, void *cb_arg,
			       unsigned long nr_to_walk);
void rs_mmput(struct mm_struct *mm);
void rs_mmput_async(struct mm_struct *mm);
void rs_zap_page_range_single(struct vm_area_struct *vma,
			      unsigned long address, unsigned long size,
			      void *details);
int rs_vm_insert_page(struct vm_area_struct *vma, unsigned long addr,
		      struct page *page);
struct device *rs_get_device(struct device *dev);
void rs_put_device(struct device *dev);
int rs_request_firmware(const struct firmware **fw, const char *name,
			struct device *dev);
int rs_firmware_request_nowarn(const struct firmware **fw, const char *name,
			       struct device *dev);
void rs_release_firmware(const struct firmware *fw);
int rs_misc_register(struct miscdevice *misc);
int rs_misc_deregister(struct miscdevice *misc);
size_t rs__copy_from_iter(void *to, size_t len, struct iov_iter *i);
size_t rs__copy_to_iter(const void *from, size_t len, struct iov_iter *i);
int rs_generic_file_open(struct inode *inode, struct file *file);
struct gendisk *rs___blk_mq_alloc_disk(struct blk_mq_tag_set *set,
				       struct queue_limits *lim,
				       void *queuedata,
				       struct lock_class_key *lkclass);
void rs_set_capacity(struct gendisk *disk, sector_t sectors);
int rs_device_add_disk(struct device *parent, struct gendisk *disk,
		       const struct attribute_group **groups);
void rs_del_gendisk(struct gendisk *disk);
void rs_blk_mq_start_request(struct request *rq);
void rs_blk_mq_end_request(struct request *rq, blk_status_t error);
int rs_blk_mq_alloc_tag_set(struct blk_mq_tag_set *set);
void rs_blk_mq_free_tag_set(struct blk_mq_tag_set *set);
blk_status_t rs_errno_to_blk_status(int err);
bool rs_queue_work_on(int cpu, struct workqueue_struct *wq,
		      struct work_struct *work);
struct rb_node *rs_rb_first(const struct rb_root *root);
struct rb_node *rs_rb_last(const struct rb_root *root);
struct rb_node *rs_rb_first_postorder(const struct rb_root *root);
struct rb_node *rs_rb_next_postorder(const struct rb_node *node);
struct rb_node *rs_rb_prev(const struct rb_node *node);
struct rb_node *rs_rb_next(const struct rb_node *node);
void rs_rb_erase(struct rb_node *node, struct rb_root *root);
void rs_rb_insert_color(struct rb_node *node, struct rb_root *root);
void rs_rb_replace_node(struct rb_node *old, struct rb_node *new,
			struct rb_root *root);
struct rb_node *rs_rb_left(const struct rb_node *node);
struct rb_node *rs_rb_right(const struct rb_node *node);
struct rb_node **rs_rb_left_ptr(struct rb_node *node);
struct rb_node **rs_rb_right_ptr(struct rb_node *node);
struct rb_node *rs_rb_root_node(const struct rb_root *root);
struct rb_node **rs_rb_root_node_ptr(struct rb_root *root);
struct workqueue_struct *rs_system_wq(void);
struct workqueue_struct *rs_system_highpri_wq(void);
struct workqueue_struct *rs_system_long_wq(void);
struct workqueue_struct *rs_system_unbound_wq(void);
struct workqueue_struct *rs_system_freezable_wq(void);
struct workqueue_struct *rs_system_power_efficient_wq(void);
struct workqueue_struct *rs_system_freezable_power_efficient_wq(void);
struct mm_struct *rs_vma_mm(struct vm_area_struct *vma);
vm_flags_t rs_vma_flags(struct vm_area_struct *vma);
unsigned long rs_vma_start(struct vm_area_struct *vma);
unsigned long rs_vma_end(struct vm_area_struct *vma);
void rs_vma_set_flags(struct vm_area_struct *vma, vm_flags_t flags);
void *rs_file_private_data(struct file *file);
void rs_file_set_private_data(struct file *file, void *data);
kuid_t rs_cred_euid(const struct cred *cred);

/* real kernel function forwarding */
int rs__printk(const char *fmt, const char *module_name, const void *args);
int rs_seq_printf(struct seq_file *m, const char *fmt, const void *args);
void rs___free_pages(struct page *page, unsigned int order);
unsigned long rs__copy_from_user(void *to, const void *from, unsigned long n);
unsigned long rs__copy_to_user(void *to, const void *from, unsigned long n);
long rs_strncpy_from_user(char *dst, const char *src, long count);
void *rs_memchr(const void *s, int c, size_t n);
size_t rs_strlen(const char *s);
void rs___init_waitqueue_head(struct wait_queue_head *wq_head,
			      const char *name, struct lock_class_key *key);
void rs_prepare_to_wait_exclusive(struct wait_queue_head *wq_head,
				  struct wait_queue_entry *wq_entry, int state);
signed long rs_schedule_timeout(signed long timeout);
void rs___wake_up(struct wait_queue_head *q, unsigned int mode,
		  int nr_exclusive, void *key);
void rs___wake_up_sync(struct wait_queue_head *q, unsigned int mode);
void rs_finish_wait(struct wait_queue_head *wq_head,
		    struct wait_queue_entry *wq_entry);
void rs_synchronize_rcu(void);
void rs___wake_up_pollfree(struct wait_queue_head *wq_head);
void rs_set_user_nice(struct task_struct *p, long nice);
int rs_sched_setscheduler_nocheck(struct task_struct *p, int policy,
				  const struct sched_param *param);
bool rs_has_capability_noaudit(struct task_struct *t, int cap);
struct pid_namespace *rs_task_active_pid_ns(struct task_struct *tsk);
int rs_wake_up_process(struct task_struct *tsk);
void rs_mutex_unlock(struct mutex *lock);
int rs_mutex_trylock(struct mutex *lock);

/* field accessors */
pid_t rs_task_pid(struct task_struct *t);
pid_t rs_task_tgid(struct task_struct *t);
struct task_struct *rs_task_group_leader(struct task_struct *t);
struct mm_struct *rs_task_mm(struct task_struct *t);
u32 rs_task_policy(struct task_struct *t);
int rs_task_normal_prio(struct task_struct *t);
const struct cred *rs_task_cred(struct task_struct *t);
int rs_page_nid(struct page *p);

#endif
