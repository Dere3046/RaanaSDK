// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#include <linux/blk-mq.h>
#include <linux/blkdev.h>
#include <linux/cred.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/firmware.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/highmem.h>
#include <linux/jump_label.h>
#include <linux/kernel.h>
#include <linux/list_lru.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/mutex.h>
#include <linux/nodemask.h>
#include <linux/pid_namespace.h>
#include <linux/poll.h>
#include <linux/rbtree.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/uio.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <linux/workqueue.h>

#include "anchor.h"
#include "core.h"
#include "type_info.h"
#include "shim.h"

struct rs_sym {
	const char *name;
	unsigned long *addr;
};

static unsigned long rs_addr__printk;
static unsigned long rs_addr_seq_printf;
static unsigned long rs_addr___free_pages;
static unsigned long rs_addr_strncpy_from_user;
static unsigned long rs_addr_memchr;
static unsigned long rs_addr_strlen;
static unsigned long rs_addr___init_waitqueue_head;
static unsigned long rs_addr_prepare_to_wait_exclusive;
static unsigned long rs_addr_schedule_timeout;
static unsigned long rs_addr___wake_up;
static unsigned long rs_addr___wake_up_sync;
static unsigned long rs_addr_finish_wait;
static unsigned long rs_addr_synchronize_rcu;
static unsigned long rs_addr___wake_up_pollfree;
static unsigned long rs_addr_set_user_nice;
static unsigned long rs_addr_sched_setscheduler_nocheck;
static unsigned long rs_addr_has_capability_noaudit;
static unsigned long rs_addr_task_active_pid_ns;
static unsigned long rs_addr_wake_up_process;
static unsigned long rs_addr_mutex_unlock;
static unsigned long rs_addr_mutex_trylock;
static unsigned long rs_addr_errname;
static unsigned long rs_addr_kvrealloc;
static unsigned long rs_addr_vrealloc;
static unsigned long rs_addr_find_vm_area;
static unsigned long rs_addr_printk;
static unsigned long rs_addr_list_lru_count_node;
static unsigned long rs_addr_list_lru_walk_node;
static unsigned long rs_addr_mmput;
static unsigned long rs_addr_mmput_async;
static unsigned long rs_addr_zap_page_range_single;
static unsigned long rs_addr_vm_insert_page;
static unsigned long rs_addr_get_device;
static unsigned long rs_addr_put_device;
static unsigned long rs_addr_request_firmware;
static unsigned long rs_addr_firmware_request_nowarn;
static unsigned long rs_addr_release_firmware;
static unsigned long rs_addr_misc_register;
static unsigned long rs_addr_misc_deregister;
static unsigned long rs_addr__copy_from_iter;
static unsigned long rs_addr__copy_to_iter;
static unsigned long rs_addr_generic_file_open;
static unsigned long rs_addr___blk_mq_alloc_disk;
static unsigned long rs_addr_set_capacity;
static unsigned long rs_addr_device_add_disk;
static unsigned long rs_addr_del_gendisk;
static unsigned long rs_addr_blk_mq_start_request;
static unsigned long rs_addr_blk_mq_end_request;
static unsigned long rs_addr_blk_mq_alloc_tag_set;
static unsigned long rs_addr_blk_mq_free_tag_set;
static unsigned long rs_addr_errno_to_blk_status;
static unsigned long rs_addr_queue_work_on;
static unsigned long rs_addr_rb_first;
static unsigned long rs_addr_rb_last;
static unsigned long rs_addr_rb_first_postorder;
static unsigned long rs_addr_rb_next_postorder;
static unsigned long rs_addr_rb_prev;
static unsigned long rs_addr_rb_next;
static unsigned long rs_addr_rb_erase;
static unsigned long rs_addr_rb_insert_color;
static unsigned long rs_addr_rb_replace_node;
static unsigned long rs_addr_system_wq;
static unsigned long rs_addr_system_highpri_wq;
static unsigned long rs_addr_system_long_wq;
static unsigned long rs_addr_system_unbound_wq;
static unsigned long rs_addr_system_freezable_wq;
static unsigned long rs_addr_system_power_efficient_wq;
static unsigned long rs_addr_system_freezable_power_efficient_wq;

static const struct rs_sym rs_syms[] = {
	{ "seq_printf", &rs_addr_seq_printf },
	{ "__free_pages", &rs_addr___free_pages },
	{ "strncpy_from_user", &rs_addr_strncpy_from_user },
	{ "memchr", &rs_addr_memchr },
	{ "strlen", &rs_addr_strlen },
	{ "__init_waitqueue_head", &rs_addr___init_waitqueue_head },
	{ "prepare_to_wait_exclusive", &rs_addr_prepare_to_wait_exclusive },
	{ "schedule_timeout", &rs_addr_schedule_timeout },
	{ "__wake_up", &rs_addr___wake_up },
	{ "__wake_up_sync", &rs_addr___wake_up_sync },
	{ "finish_wait", &rs_addr_finish_wait },
	{ "synchronize_rcu", &rs_addr_synchronize_rcu },
	{ "__wake_up_pollfree", &rs_addr___wake_up_pollfree },
	{ "set_user_nice", &rs_addr_set_user_nice },
	{ "sched_setscheduler_nocheck", &rs_addr_sched_setscheduler_nocheck },
	{ "has_capability_noaudit", &rs_addr_has_capability_noaudit },
	{ "task_active_pid_ns", &rs_addr_task_active_pid_ns },
	{ "wake_up_process", &rs_addr_wake_up_process },
	{ "mutex_unlock", &rs_addr_mutex_unlock },
	{ "mutex_trylock", &rs_addr_mutex_trylock },
};

enum rs_field_id {
	RS_FIELD_TASK_PID,
	RS_FIELD_TASK_TGID,
	RS_FIELD_TASK_GROUP_LEADER,
	RS_FIELD_TASK_MM,
	RS_FIELD_TASK_POLICY,
	RS_FIELD_TASK_NORMAL_PRIO,
	RS_FIELD_TASK_CRED,
	RS_FIELD_PAGE_NID,
	RS_FIELD_NUM,
};

struct rs_field {
	const char *type_name;
	const char *member_name;
	u32 offset;
	u32 size;
	bool valid;
};

static struct rs_field rs_fields[RS_FIELD_NUM] = {
	{ "task_struct", "pid",          0, 4, false },
	{ "task_struct", "tgid",         0, 4, false },
	{ "task_struct", "group_leader", 0, 8, false },
	{ "task_struct", "mm",           0, 8, false },
	{ "task_struct", "policy",       0, 4, false },
	{ "task_struct", "normal_prio",  0, 4, false },
	{ "task_struct", "cred",         0, 8, false },
	{ "page",        "nid",          0, 4, false },
};

__nocfi unsigned long rs_resolve_name(const char *name)
{
	if (kallrecon_klp)
		return kallrecon_klp(name);
	return kallsyms_name_to_addr(name);
}

static void rs_field_resolve_btf(struct rs_field *f)
{
	struct ti_ctx *ctx = ti_base();
	u32 id, bit_off, bit_sz;

	if (ctx && !ti_type_by_name(ctx, f->type_name, 0, &id) &&
	    !ti_member_off(ctx, id, f->member_name, &bit_off, &bit_sz)) {
		f->offset = bit_off / 8;
		f->valid = true;
	}
}

static void rs_task_anchor_offs(void)
{
	struct ti_boot_args args;
	struct ti_task_offs offs;

	memset(&args, 0, sizeof(args));
	args.pid = current->pid;
	args.tgid = current->tgid;
	strscpy((char *)args.comm, current->comm, sizeof(args.comm));
	args.ref_pid = 0;
	args.ref_tgid = 0;

	if (ti_bootstrap_task(&args, &offs))
		return;

	if (offs.off_pid) {
		rs_fields[RS_FIELD_TASK_PID].offset = offs.off_pid;
		rs_fields[RS_FIELD_TASK_PID].valid = true;
		rs_fields[RS_FIELD_TASK_TGID].offset = offs.off_pid + 4;
		rs_fields[RS_FIELD_TASK_TGID].valid = true;
	}
	if (offs.off_cred) {
		rs_fields[RS_FIELD_TASK_CRED].offset = offs.off_cred;
		rs_fields[RS_FIELD_TASK_CRED].valid = true;
	}
	if (offs.off_mm) {
		rs_fields[RS_FIELD_TASK_MM].offset = offs.off_mm;
		rs_fields[RS_FIELD_TASK_MM].valid = true;
	}
}

static void rs_task_compile_offs(void)
{
#define RS_TASK_COMPILE(id, field) do { \
	if (!rs_fields[id].valid) { \
		rs_fields[id].offset = offsetof(struct task_struct, field); \
		rs_fields[id].valid = true; \
	} \
} while (0)

	RS_TASK_COMPILE(RS_FIELD_TASK_PID, pid);
	RS_TASK_COMPILE(RS_FIELD_TASK_TGID, tgid);
	RS_TASK_COMPILE(RS_FIELD_TASK_GROUP_LEADER, group_leader);
	RS_TASK_COMPILE(RS_FIELD_TASK_MM, mm);
	RS_TASK_COMPILE(RS_FIELD_TASK_POLICY, policy);
	RS_TASK_COMPILE(RS_FIELD_TASK_NORMAL_PRIO, normal_prio);
	RS_TASK_COMPILE(RS_FIELD_TASK_CRED, cred);

#undef RS_TASK_COMPILE
}

int rs_shim_init(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(rs_syms); i++) {
		unsigned long addr = rs_resolve_name(rs_syms[i].name);

		if (!addr) {
			pr_err("[rust_support] missing symbol %s\n",
			       rs_syms[i].name);
			return -ENOENT;
		}
		*rs_syms[i].addr = addr;
	}

	rs_addr__printk = rs_resolve_name("_printk");
	rs_addr_printk = rs_resolve_name("printk");
	rs_addr_errname = rs_resolve_name("errname");
	rs_addr_find_vm_area = rs_resolve_name("find_vm_area");
	rs_addr_list_lru_count_node = rs_resolve_name("list_lru_count_node");
	rs_addr_list_lru_walk_node = rs_resolve_name("list_lru_walk_node");
	rs_addr_mmput = rs_resolve_name("mmput");
	rs_addr_mmput_async = rs_resolve_name("mmput_async");
	rs_addr_zap_page_range_single = rs_resolve_name("zap_page_range_single");
	rs_addr_vm_insert_page = rs_resolve_name("vm_insert_page");
	rs_addr_get_device = rs_resolve_name("get_device");
	rs_addr_put_device = rs_resolve_name("put_device");
	rs_addr_request_firmware = rs_resolve_name("request_firmware");
	rs_addr_firmware_request_nowarn = rs_resolve_name("firmware_request_nowarn");
	rs_addr_release_firmware = rs_resolve_name("release_firmware");
	rs_addr_misc_register = rs_resolve_name("misc_register");
	rs_addr_misc_deregister = rs_resolve_name("misc_deregister");
	rs_addr__copy_from_iter = rs_resolve_name("_copy_from_iter");
	rs_addr__copy_to_iter = rs_resolve_name("_copy_to_iter");
	rs_addr_generic_file_open = rs_resolve_name("generic_file_open");
	rs_addr___blk_mq_alloc_disk = rs_resolve_name("__blk_mq_alloc_disk");
	rs_addr_set_capacity = rs_resolve_name("set_capacity");
	rs_addr_device_add_disk = rs_resolve_name("device_add_disk");
	rs_addr_del_gendisk = rs_resolve_name("del_gendisk");
	rs_addr_blk_mq_start_request = rs_resolve_name("blk_mq_start_request");
	rs_addr_blk_mq_end_request = rs_resolve_name("blk_mq_end_request");
	rs_addr_blk_mq_alloc_tag_set = rs_resolve_name("blk_mq_alloc_tag_set");
	rs_addr_blk_mq_free_tag_set = rs_resolve_name("blk_mq_free_tag_set");
	rs_addr_errno_to_blk_status = rs_resolve_name("errno_to_blk_status");
	rs_addr_queue_work_on = rs_resolve_name("queue_work_on");
	rs_addr_rb_first = rs_resolve_name("rb_first");
	rs_addr_rb_last = rs_resolve_name("rb_last");
	rs_addr_rb_first_postorder = rs_resolve_name("rb_first_postorder");
	rs_addr_rb_next_postorder = rs_resolve_name("rb_next_postorder");
	rs_addr_rb_prev = rs_resolve_name("rb_prev");
	rs_addr_rb_next = rs_resolve_name("rb_next");
	rs_addr_rb_erase = rs_resolve_name("rb_erase");
	rs_addr_rb_insert_color = rs_resolve_name("rb_insert_color");
	rs_addr_rb_replace_node = rs_resolve_name("rb_replace_node");
	rs_addr_system_wq = rs_resolve_name("system_wq");
	rs_addr_system_highpri_wq = rs_resolve_name("system_highpri_wq");
	rs_addr_system_long_wq = rs_resolve_name("system_long_wq");
	rs_addr_system_unbound_wq = rs_resolve_name("system_unbound_wq");
	rs_addr_system_freezable_wq = rs_resolve_name("system_freezable_wq");
	rs_addr_system_power_efficient_wq = rs_resolve_name("system_power_efficient_wq");
	rs_addr_system_freezable_power_efficient_wq =
		rs_resolve_name("system_freezable_power_efficient_wq");
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	rs_addr_kvrealloc = rs_resolve_name("kvrealloc_noprof");
	if (!rs_addr_kvrealloc)
		rs_addr_kvrealloc = rs_resolve_name("kvrealloc");
	rs_addr_vrealloc = rs_resolve_name("vrealloc_noprof");
	if (!rs_addr_vrealloc)
		rs_addr_vrealloc = rs_resolve_name("vrealloc");
#endif

	for (i = 0; i < RS_FIELD_NUM; i++)
		rs_field_resolve_btf(&rs_fields[i]);

	rs_task_anchor_offs();
	rs_task_compile_offs();

	if (!rs_fields[RS_FIELD_TASK_PID].valid ||
	    !rs_fields[RS_FIELD_TASK_TGID].valid ||
	    !rs_fields[RS_FIELD_TASK_GROUP_LEADER].valid ||
	    !rs_fields[RS_FIELD_TASK_MM].valid ||
	    !rs_fields[RS_FIELD_TASK_POLICY].valid ||
	    !rs_fields[RS_FIELD_TASK_NORMAL_PRIO].valid ||
	    !rs_fields[RS_FIELD_TASK_CRED].valid) {
		pr_err("[rust_support] task offset init failed\n");
		return -ENOENT;
	}

	pr_info("[rust_support] shim ready, %zu symbols\n",
		ARRAY_SIZE(rs_syms));
	return 0;
}

void rs_shim_exit(void)
{
}

static size_t rs_vmalloc_size(const void *p)
{
	struct vm_struct *vm = rs_find_vm_area(p);

	if (vm)
		return get_vm_area_size(vm);
	return 0;
}

__nocfi const char *rs_errname(int err)
{
	typedef const char *(*fn_t)(int);

	if (!rs_addr_errname)
		return NULL;
	return ((fn_t)rs_addr_errname)(err);
}

__nocfi void *rs_kvrealloc(const void *p, size_t size, unsigned int flags)
{
	if (rs_addr_kvrealloc) {
		typedef void *(*fn_t)(const void *, size_t, gfp_t);

		return ((fn_t)rs_addr_kvrealloc)(p, size, (gfp_t)flags);
	}

	if (!p)
		return kvmalloc(size, (gfp_t)flags);
	if (size == 0) {
		kvfree(p);
		return NULL;
	}
	if (is_vmalloc_addr(p)) {
		size_t old_size = rs_vmalloc_size(p);
		void *newp;

		if (!old_size)
			return NULL;
		newp = kvmalloc(size, (gfp_t)flags);
		if (!newp)
			return NULL;
		memcpy(newp, p, min(old_size, size));
		kvfree(p);
		return newp;
	}
	return krealloc(p, size, (gfp_t)flags);
}

__nocfi void *rs_vrealloc(const void *p, size_t size, unsigned int flags)
{
	if (rs_addr_vrealloc) {
		typedef void *(*fn_t)(const void *, size_t, gfp_t);

		return ((fn_t)rs_addr_vrealloc)(p, size, (gfp_t)flags);
	}

	if (!p)
		return __vmalloc(size, (gfp_t)flags);
	if (size == 0) {
		vfree(p);
		return NULL;
	}
	{
		size_t old_size = rs_vmalloc_size(p);
		void *newp;

		if (!old_size)
			return NULL;
		newp = __vmalloc(size, (gfp_t)flags);
		if (!newp)
			return NULL;
		memcpy(newp, p, min(old_size, size));
		vfree(p);
		return newp;
	}
}

__nocfi struct vm_struct *rs_find_vm_area(const void *addr)
{
	typedef struct vm_struct *(*fn_t)(const void *);

	if (!rs_addr_find_vm_area)
		return NULL;
	return ((fn_t)rs_addr_find_vm_area)(addr);
}

__nocfi unsigned long rs_list_lru_count(struct list_lru *lru)
{
	typedef unsigned long (*fn_t)(struct list_lru *, int);
	int nid;
	unsigned long count = 0;

	if (!rs_addr_list_lru_count_node)
		return 0;

	for_each_node_state(nid, N_NORMAL_MEMORY)
		count += ((fn_t)rs_addr_list_lru_count_node)(lru, nid);

	return count;
}

__nocfi unsigned long rs_list_lru_walk(struct list_lru *lru,
				       list_lru_walk_cb isolate, void *cb_arg,
				       unsigned long nr_to_walk)
{
	typedef unsigned long (*fn_t)(struct list_lru *, int,
				      list_lru_walk_cb, void *, unsigned long *);
	int nid;
	unsigned long isolated = 0;

	if (!rs_addr_list_lru_walk_node)
		return 0;

	for_each_node_state(nid, N_NORMAL_MEMORY) {
		isolated += ((fn_t)rs_addr_list_lru_walk_node)(lru, nid,
				isolate, cb_arg, &nr_to_walk);
		if (!nr_to_walk)
			break;
	}

	return isolated;
}

__nocfi void rs_mmput(struct mm_struct *mm)
{
	typedef void (*fn_t)(struct mm_struct *);

	if (rs_addr_mmput)
		((fn_t)rs_addr_mmput)(mm);
}

__nocfi void rs_mmput_async(struct mm_struct *mm)
{
	typedef void (*fn_t)(struct mm_struct *);

	if (rs_addr_mmput_async)
		((fn_t)rs_addr_mmput_async)(mm);
}

__nocfi void rs_zap_page_range_single(struct vm_area_struct *vma,
				      unsigned long address,
				      unsigned long size, void *details)
{
	typedef void (*fn_t)(struct vm_area_struct *, unsigned long,
			     unsigned long, void *);

	if (rs_addr_zap_page_range_single)
		((fn_t)rs_addr_zap_page_range_single)(vma, address, size, details);
}

__nocfi int rs_vm_insert_page(struct vm_area_struct *vma, unsigned long addr,
			      struct page *page)
{
	typedef int (*fn_t)(struct vm_area_struct *, unsigned long, struct page *);

	if (!rs_addr_vm_insert_page)
		return -ENOSYS;
	return ((fn_t)rs_addr_vm_insert_page)(vma, addr, page);
}

__nocfi struct device *rs_get_device(struct device *dev)
{
	typedef struct device *(*fn_t)(struct device *);

	if (!rs_addr_get_device)
		return NULL;
	return ((fn_t)rs_addr_get_device)(dev);
}

__nocfi void rs_put_device(struct device *dev)
{
	typedef void (*fn_t)(struct device *);

	if (rs_addr_put_device)
		((fn_t)rs_addr_put_device)(dev);
}

__nocfi int rs_request_firmware(const struct firmware **fw, const char *name,
				struct device *dev)
{
	typedef int (*fn_t)(const struct firmware **, const char *,
			    struct device *);

	if (!rs_addr_request_firmware)
		return -ENOSYS;
	return ((fn_t)rs_addr_request_firmware)(fw, name, dev);
}

__nocfi int rs_firmware_request_nowarn(const struct firmware **fw,
				       const char *name, struct device *dev)
{
	typedef int (*fn_t)(const struct firmware **, const char *,
			    struct device *);

	if (!rs_addr_firmware_request_nowarn)
		return -ENOSYS;
	return ((fn_t)rs_addr_firmware_request_nowarn)(fw, name, dev);
}

__nocfi void rs_release_firmware(const struct firmware *fw)
{
	typedef void (*fn_t)(const struct firmware *);

	if (rs_addr_release_firmware)
		((fn_t)rs_addr_release_firmware)(fw);
}

__nocfi int rs_misc_register(struct miscdevice *misc)
{
	typedef int (*fn_t)(struct miscdevice *);

	if (!rs_addr_misc_register)
		return -ENOSYS;
	return ((fn_t)rs_addr_misc_register)(misc);
}

__nocfi int rs_misc_deregister(struct miscdevice *misc)
{
	typedef int (*fn_t)(struct miscdevice *);

	if (!rs_addr_misc_deregister)
		return -ENOSYS;
	return ((fn_t)rs_addr_misc_deregister)(misc);
}

__nocfi size_t rs__copy_from_iter(void *to, size_t len, struct iov_iter *i)
{
	typedef size_t (*fn_t)(void *, size_t, struct iov_iter *);

	if (!rs_addr__copy_from_iter)
		return 0;
	return ((fn_t)rs_addr__copy_from_iter)(to, len, i);
}

__nocfi size_t rs__copy_to_iter(const void *from, size_t len, struct iov_iter *i)
{
	typedef size_t (*fn_t)(const void *, size_t, struct iov_iter *);

	if (!rs_addr__copy_to_iter)
		return 0;
	return ((fn_t)rs_addr__copy_to_iter)(from, len, i);
}

__nocfi int rs_generic_file_open(struct inode *inode, struct file *file)
{
	typedef int (*fn_t)(struct inode *, struct file *);

	if (!rs_addr_generic_file_open)
		return -ENOSYS;
	return ((fn_t)rs_addr_generic_file_open)(inode, file);
}

__nocfi struct gendisk *rs___blk_mq_alloc_disk(struct blk_mq_tag_set *set,
					       struct queue_limits *lim,
					       void *queuedata,
					       struct lock_class_key *lkclass)
{
	typedef struct gendisk *(*fn_t)(struct blk_mq_tag_set *,
					struct queue_limits *, void *,
					struct lock_class_key *);

	if (!rs_addr___blk_mq_alloc_disk)
		return NULL;
	return ((fn_t)rs_addr___blk_mq_alloc_disk)(set, lim, queuedata, lkclass);
}

__nocfi void rs_set_capacity(struct gendisk *disk, sector_t sectors)
{
	typedef void (*fn_t)(struct gendisk *, sector_t);

	if (rs_addr_set_capacity)
		((fn_t)rs_addr_set_capacity)(disk, sectors);
}

__nocfi int rs_device_add_disk(struct device *parent, struct gendisk *disk,
			       const struct attribute_group **groups)
{
	typedef int (*fn_t)(struct device *, struct gendisk *,
			    const struct attribute_group **);

	if (!rs_addr_device_add_disk)
		return -ENOSYS;
	return ((fn_t)rs_addr_device_add_disk)(parent, disk, groups);
}

__nocfi void rs_del_gendisk(struct gendisk *disk)
{
	typedef void (*fn_t)(struct gendisk *);

	if (rs_addr_del_gendisk)
		((fn_t)rs_addr_del_gendisk)(disk);
}

__nocfi void rs_blk_mq_start_request(struct request *rq)
{
	typedef void (*fn_t)(struct request *);

	if (rs_addr_blk_mq_start_request)
		((fn_t)rs_addr_blk_mq_start_request)(rq);
}

__nocfi void rs_blk_mq_end_request(struct request *rq, blk_status_t error)
{
	typedef void (*fn_t)(struct request *, blk_status_t);

	if (rs_addr_blk_mq_end_request)
		((fn_t)rs_addr_blk_mq_end_request)(rq, error);
}

__nocfi int rs_blk_mq_alloc_tag_set(struct blk_mq_tag_set *set)
{
	typedef int (*fn_t)(struct blk_mq_tag_set *);

	if (!rs_addr_blk_mq_alloc_tag_set)
		return -ENOSYS;
	return ((fn_t)rs_addr_blk_mq_alloc_tag_set)(set);
}

__nocfi void rs_blk_mq_free_tag_set(struct blk_mq_tag_set *set)
{
	typedef void (*fn_t)(struct blk_mq_tag_set *);

	if (rs_addr_blk_mq_free_tag_set)
		((fn_t)rs_addr_blk_mq_free_tag_set)(set);
}

__nocfi blk_status_t rs_errno_to_blk_status(int err)
{
	typedef blk_status_t (*fn_t)(int);

	if (!rs_addr_errno_to_blk_status)
		return 0;
	return ((fn_t)rs_addr_errno_to_blk_status)(err);
}

__nocfi bool rs_queue_work_on(int cpu, struct workqueue_struct *wq,
			      struct work_struct *work)
{
	typedef bool (*fn_t)(int, struct workqueue_struct *, struct work_struct *);

	if (!rs_addr_queue_work_on)
		return false;
	return ((fn_t)rs_addr_queue_work_on)(cpu, wq, work);
}

__nocfi struct rb_node *rs_rb_first(const struct rb_root *root)
{
	typedef struct rb_node *(*fn_t)(const struct rb_root *);

	if (!rs_addr_rb_first)
		return NULL;
	return ((fn_t)rs_addr_rb_first)(root);
}

__nocfi struct rb_node *rs_rb_last(const struct rb_root *root)
{
	typedef struct rb_node *(*fn_t)(const struct rb_root *);

	if (!rs_addr_rb_last)
		return NULL;
	return ((fn_t)rs_addr_rb_last)(root);
}

__nocfi struct rb_node *rs_rb_first_postorder(const struct rb_root *root)
{
	typedef struct rb_node *(*fn_t)(const struct rb_root *);

	if (!rs_addr_rb_first_postorder)
		return NULL;
	return ((fn_t)rs_addr_rb_first_postorder)(root);
}

__nocfi struct rb_node *rs_rb_next_postorder(const struct rb_node *node)
{
	typedef struct rb_node *(*fn_t)(const struct rb_node *);

	if (!rs_addr_rb_next_postorder)
		return NULL;
	return ((fn_t)rs_addr_rb_next_postorder)(node);
}

__nocfi struct rb_node *rs_rb_prev(const struct rb_node *node)
{
	typedef struct rb_node *(*fn_t)(const struct rb_node *);

	if (!rs_addr_rb_prev)
		return NULL;
	return ((fn_t)rs_addr_rb_prev)(node);
}

__nocfi struct rb_node *rs_rb_next(const struct rb_node *node)
{
	typedef struct rb_node *(*fn_t)(const struct rb_node *);

	if (!rs_addr_rb_next)
		return NULL;
	return ((fn_t)rs_addr_rb_next)(node);
}

__nocfi void rs_rb_erase(struct rb_node *node, struct rb_root *root)
{
	typedef void (*fn_t)(struct rb_node *, struct rb_root *);

	if (rs_addr_rb_erase)
		((fn_t)rs_addr_rb_erase)(node, root);
}

__nocfi void rs_rb_insert_color(struct rb_node *node, struct rb_root *root)
{
	typedef void (*fn_t)(struct rb_node *, struct rb_root *);

	if (rs_addr_rb_insert_color)
		((fn_t)rs_addr_rb_insert_color)(node, root);
}

__nocfi void rs_rb_replace_node(struct rb_node *old, struct rb_node *new,
				struct rb_root *root)
{
	typedef void (*fn_t)(struct rb_node *, struct rb_node *,
			     struct rb_root *);

	if (rs_addr_rb_replace_node)
		((fn_t)rs_addr_rb_replace_node)(old, new, root);
}

__nocfi struct workqueue_struct *rs_system_wq(void)
{
	if (!rs_addr_system_wq)
		return NULL;
	return *(struct workqueue_struct **)rs_addr_system_wq;
}

__nocfi struct workqueue_struct *rs_system_highpri_wq(void)
{
	if (!rs_addr_system_highpri_wq)
		return NULL;
	return *(struct workqueue_struct **)rs_addr_system_highpri_wq;
}

__nocfi struct workqueue_struct *rs_system_long_wq(void)
{
	if (!rs_addr_system_long_wq)
		return NULL;
	return *(struct workqueue_struct **)rs_addr_system_long_wq;
}

__nocfi struct workqueue_struct *rs_system_unbound_wq(void)
{
	if (!rs_addr_system_unbound_wq)
		return NULL;
	return *(struct workqueue_struct **)rs_addr_system_unbound_wq;
}

__nocfi struct workqueue_struct *rs_system_freezable_wq(void)
{
	if (!rs_addr_system_freezable_wq)
		return NULL;
	return *(struct workqueue_struct **)rs_addr_system_freezable_wq;
}

__nocfi struct workqueue_struct *rs_system_power_efficient_wq(void)
{
	if (!rs_addr_system_power_efficient_wq)
		return NULL;
	return *(struct workqueue_struct **)rs_addr_system_power_efficient_wq;
}

__nocfi struct workqueue_struct *rs_system_freezable_power_efficient_wq(void)
{
	if (!rs_addr_system_freezable_power_efficient_wq)
		return NULL;
	return *(struct workqueue_struct **)rs_addr_system_freezable_power_efficient_wq;
}

__nocfi struct rb_node *rs_rb_left(const struct rb_node *node)
{
	return node->rb_left;
}

__nocfi struct rb_node *rs_rb_right(const struct rb_node *node)
{
	return node->rb_right;
}

__nocfi struct rb_node **rs_rb_left_ptr(struct rb_node *node)
{
	return &node->rb_left;
}

__nocfi struct rb_node **rs_rb_right_ptr(struct rb_node *node)
{
	return &node->rb_right;
}

__nocfi struct rb_node *rs_rb_root_node(const struct rb_root *root)
{
	return root->rb_node;
}

__nocfi struct rb_node **rs_rb_root_node_ptr(struct rb_root *root)
{
	return &root->rb_node;
}

__nocfi struct mm_struct *rs_vma_mm(struct vm_area_struct *vma)
{
	return vma->vm_mm;
}

__nocfi vm_flags_t rs_vma_flags(struct vm_area_struct *vma)
{
	return vma->vm_flags;
}

__nocfi unsigned long rs_vma_start(struct vm_area_struct *vma)
{
	return vma->vm_start;
}

__nocfi unsigned long rs_vma_end(struct vm_area_struct *vma)
{
	return vma->vm_end;
}

__nocfi void rs_vma_set_flags(struct vm_area_struct *vma, vm_flags_t flags)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	vma->__vm_flags = flags;
#else
	vma->vm_flags = flags;
#endif
}

__nocfi void *rs_file_private_data(struct file *file)
{
	return file->private_data;
}

__nocfi void rs_file_set_private_data(struct file *file, void *data)
{
	file->private_data = data;
}

__nocfi kuid_t rs_cred_euid(const struct cred *cred)
{
	return cred->euid;
}

__nocfi int rs__printk(const char *fmt, const char *module_name,
		       const void *args)
{
	typedef int (*fn_t)(const char *, ...);

	if (rs_addr__printk)
		return ((fn_t)rs_addr__printk)(fmt, module_name, args);
	if (rs_addr_printk)
		return ((fn_t)rs_addr_printk)(fmt, module_name, args);
	return 0;
}

__nocfi int rs_seq_printf(struct seq_file *m, const char *fmt,
			  const void *args)
{
	typedef int (*fn_t)(struct seq_file *, const char *, ...);

	return ((fn_t)rs_addr_seq_printf)(m, fmt, args);
}

__nocfi void rs___free_pages(struct page *page, unsigned int order)
{
	typedef void (*fn_t)(struct page *, unsigned int);

	((fn_t)rs_addr___free_pages)(page, order);
}

__nocfi unsigned long rs__copy_from_user(void *to, const void *from,
					 unsigned long n)
{
	return copy_from_user(to, (const void __user *)from, n);
}

__nocfi unsigned long rs__copy_to_user(void *to, const void *from,
				       unsigned long n)
{
	return copy_to_user((void __user *)to, from, n);
}

__nocfi long rs_strncpy_from_user(char *dst, const char *src, long count)
{
	typedef long (*fn_t)(char *, const char *, long);

	return ((fn_t)rs_addr_strncpy_from_user)(dst, src, count);
}

__nocfi void *rs_memchr(const void *s, int c, size_t n)
{
	typedef void *(*fn_t)(const void *, int, size_t);

	return ((fn_t)rs_addr_memchr)(s, c, n);
}

__nocfi size_t rs_strlen(const char *s)
{
	typedef size_t (*fn_t)(const char *);

	return ((fn_t)rs_addr_strlen)(s);
}

__nocfi void rs___init_waitqueue_head(struct wait_queue_head *wq_head,
				      const char *name,
				      struct lock_class_key *key)
{
	typedef void (*fn_t)(struct wait_queue_head *, const char *,
			     struct lock_class_key *);

	((fn_t)rs_addr___init_waitqueue_head)(wq_head, name, key);
}

__nocfi void rs_prepare_to_wait_exclusive(struct wait_queue_head *wq_head,
					  struct wait_queue_entry *wq_entry,
					  int state)
{
	typedef void (*fn_t)(struct wait_queue_head *,
			     struct wait_queue_entry *, int);

	((fn_t)rs_addr_prepare_to_wait_exclusive)(wq_head, wq_entry, state);
}

__nocfi signed long rs_schedule_timeout(signed long timeout)
{
	typedef signed long (*fn_t)(signed long);

	return ((fn_t)rs_addr_schedule_timeout)(timeout);
}

__nocfi void rs___wake_up(struct wait_queue_head *q, unsigned int mode,
			  int nr_exclusive, void *key)
{
	typedef void (*fn_t)(struct wait_queue_head *, unsigned int, int,
			     void *);

	((fn_t)rs_addr___wake_up)(q, mode, nr_exclusive, key);
}

__nocfi void rs___wake_up_sync(struct wait_queue_head *q, unsigned int mode)
{
	typedef void (*fn_t)(struct wait_queue_head *, unsigned int);

	((fn_t)rs_addr___wake_up_sync)(q, mode);
}

__nocfi void rs_finish_wait(struct wait_queue_head *wq_head,
			    struct wait_queue_entry *wq_entry)
{
	typedef void (*fn_t)(struct wait_queue_head *,
			     struct wait_queue_entry *);

	((fn_t)rs_addr_finish_wait)(wq_head, wq_entry);
}

__nocfi void rs_synchronize_rcu(void)
{
	typedef void (*fn_t)(void);

	((fn_t)rs_addr_synchronize_rcu)();
}

__nocfi void rs___wake_up_pollfree(struct wait_queue_head *wq_head)
{
	typedef void (*fn_t)(struct wait_queue_head *);

	((fn_t)rs_addr___wake_up_pollfree)(wq_head);
}

__nocfi void rs_set_user_nice(struct task_struct *p, long nice)
{
	typedef void (*fn_t)(struct task_struct *, long);

	((fn_t)rs_addr_set_user_nice)(p, nice);
}

__nocfi int rs_sched_setscheduler_nocheck(struct task_struct *p, int policy,
					  const struct sched_param *param)
{
	typedef int (*fn_t)(struct task_struct *, int,
			    const struct sched_param *);

	return ((fn_t)rs_addr_sched_setscheduler_nocheck)(p, policy, param);
}

__nocfi bool rs_has_capability_noaudit(struct task_struct *t, int cap)
{
	typedef bool (*fn_t)(struct task_struct *, int);

	return ((fn_t)rs_addr_has_capability_noaudit)(t, cap);
}

__nocfi struct pid_namespace *rs_task_active_pid_ns(struct task_struct *tsk)
{
	typedef struct pid_namespace *(*fn_t)(struct task_struct *);

	return ((fn_t)rs_addr_task_active_pid_ns)(tsk);
}

__nocfi int rs_wake_up_process(struct task_struct *tsk)
{
	typedef int (*fn_t)(struct task_struct *);

	return ((fn_t)rs_addr_wake_up_process)(tsk);
}

__nocfi void rs_mutex_unlock(struct mutex *lock)
{
	typedef void (*fn_t)(struct mutex *);

	((fn_t)rs_addr_mutex_unlock)(lock);
}

__nocfi int rs_mutex_trylock(struct mutex *lock)
{
	typedef int (*fn_t)(struct mutex *);

	return ((fn_t)rs_addr_mutex_trylock)(lock);
}

pid_t rs_task_pid(struct task_struct *t)
{
	if (!rs_fields[RS_FIELD_TASK_PID].valid)
		return 0;
	return *(pid_t *)((char *)t + rs_fields[RS_FIELD_TASK_PID].offset);
}

pid_t rs_task_tgid(struct task_struct *t)
{
	if (!rs_fields[RS_FIELD_TASK_TGID].valid)
		return 0;
	return *(pid_t *)((char *)t + rs_fields[RS_FIELD_TASK_TGID].offset);
}

struct task_struct *rs_task_group_leader(struct task_struct *t)
{
	if (!rs_fields[RS_FIELD_TASK_GROUP_LEADER].valid)
		return NULL;
	return *(struct task_struct **)((char *)t +
		rs_fields[RS_FIELD_TASK_GROUP_LEADER].offset);
}

struct mm_struct *rs_task_mm(struct task_struct *t)
{
	if (!rs_fields[RS_FIELD_TASK_MM].valid)
		return NULL;
	return *(struct mm_struct **)((char *)t +
		rs_fields[RS_FIELD_TASK_MM].offset);
}

u32 rs_task_policy(struct task_struct *t)
{
	if (!rs_fields[RS_FIELD_TASK_POLICY].valid)
		return 0;
	return READ_ONCE(*(u32 *)((char *)t +
		rs_fields[RS_FIELD_TASK_POLICY].offset));
}

int rs_task_normal_prio(struct task_struct *t)
{
	if (!rs_fields[RS_FIELD_TASK_NORMAL_PRIO].valid)
		return 0;
	return READ_ONCE(*(int *)((char *)t +
		rs_fields[RS_FIELD_TASK_NORMAL_PRIO].offset));
}

const struct cred *rs_task_cred(struct task_struct *t)
{
	if (!rs_fields[RS_FIELD_TASK_CRED].valid)
		return NULL;
	return *(const struct cred **)((char *)t +
		rs_fields[RS_FIELD_TASK_CRED].offset);
}

int rs_page_nid(struct page *p)
{
	if (!rs_fields[RS_FIELD_PAGE_NID].valid)
		return -1;
	return *(int *)((char *)p + rs_fields[RS_FIELD_PAGE_NID].offset);
}
