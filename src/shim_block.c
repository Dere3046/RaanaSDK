// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#include <linux/blk-mq.h>
#include <linux/blkdev.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0)
#include <linux/genhd.h>
#endif

#include "shim.h"
#include "shim_block.h"

/*
 * Convert between the Rust-facing opaque function pointer types and the real
 * kernel callback types without tripping -Wcast-function-type.  Function
 * pointers are all pointer-sized on every supported kernel, and the Rust side
 * only stores/returns these callbacks as opaque pointers.
 */
#define rs_fn_to(type, expr)						\
	({								\
		type out;						\
		__builtin_memcpy(&out, &(expr), sizeof(out));		\
		out;							\
	})

#define rs_fn_assign(field, value)					\
	do {								\
		typeof(field) tmp;					\
		__builtin_memcpy(&tmp, &(value), sizeof(tmp));		\
		(field) = tmp;						\
	} while (0)


/* Zero-initialized fops used by Rust GenDisk before it overrides fields. */
static const struct block_device_operations rs_bdo_zero;

__nocfi struct gendisk *rs_blk_mq_alloc_disk_with_limits(
		struct blk_mq_tag_set *tag_set, bool rotational,
		u32 logical_block_size, u32 physical_block_size,
		void *queuedata, struct lock_class_key *lkclass)
{
	struct queue_limits lim = { };

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	if (rotational)
		lim.features = (rs_blk_features_t)BLK_FEAT_ROTATIONAL;
#endif
	lim.logical_block_size = logical_block_size;
	lim.physical_block_size = physical_block_size;

	return rs___blk_mq_alloc_disk(tag_set, &lim, queuedata, lkclass);
}

__nocfi const struct blk_mq_ops *rs_blk_mq_ops_alloc(
		rs_blk_mq_queue_rq_callback_t queue_rq,
		rs_blk_mq_commit_rqs_callback_t commit_rqs,
		rs_blk_mq_poll_callback_t poll,
		rs_blk_mq_complete_callback_t complete,
		rs_blk_mq_init_hctx_callback_t init_hctx,
		rs_blk_mq_exit_hctx_callback_t exit_hctx,
		rs_blk_mq_init_request_callback_t init_request,
		rs_blk_mq_exit_request_callback_t exit_request)
{
	struct blk_mq_ops *ops;

	ops = kzalloc(sizeof(*ops), GFP_KERNEL);
	if (!ops)
		return NULL;

	rs_fn_assign(ops->queue_rq, queue_rq);
	rs_fn_assign(ops->commit_rqs, commit_rqs);
	rs_fn_assign(ops->poll, poll);
	rs_fn_assign(ops->complete, complete);
	rs_fn_assign(ops->init_hctx, init_hctx);
	rs_fn_assign(ops->exit_hctx, exit_hctx);
	rs_fn_assign(ops->init_request, init_request);
	rs_fn_assign(ops->exit_request, exit_request);

	return ops;
}

__nocfi struct blk_mq_tag_set *rs_blk_mq_tag_set_alloc(
		const struct blk_mq_ops *ops, u32 nr_hw_queues, u32 num_tags,
		u32 num_maps, u32 cmd_size, u32 flags, int numa_node,
		u32 timeout, void *driver_data)
{
	struct blk_mq_tag_set *set;
	int ret;

	if (!ops)
		return NULL;

	set = kzalloc(sizeof(*set), GFP_KERNEL);
	if (!set) {
		kfree((void *)ops);
		return NULL;
	}

	set->ops = ops;
	set->nr_hw_queues = nr_hw_queues;
	set->nr_maps = num_maps;
	set->queue_depth = num_tags;
	set->cmd_size = cmd_size;
	set->flags = flags;
	set->numa_node = numa_node;
	set->timeout = timeout;
	set->driver_data = driver_data;

	ret = rs_blk_mq_alloc_tag_set(set);
	if (ret) {
		kfree(set);
		kfree((void *)ops);
		return NULL;
	}

	return set;
}

__nocfi void rs_blk_mq_tag_set_free(struct blk_mq_tag_set *set)
{
	const struct blk_mq_ops *ops;

	if (!set)
		return;

	ops = set->ops;
	rs_blk_mq_free_tag_set(set);
	kfree(set);
	kfree((void *)ops);
}

/* blk_mq_queue_data accessors */

__nocfi struct request *rs_blk_mq_queue_data_rq(
		const struct blk_mq_queue_data *obj)
{
	return obj->rq;
}

__nocfi void rs_blk_mq_queue_data_set_rq(struct blk_mq_queue_data *obj,
					 struct request *value)
{
	obj->rq = value;
}

__nocfi bool rs_blk_mq_queue_data_last(const struct blk_mq_queue_data *obj)
{
	return obj->last;
}

__nocfi void rs_blk_mq_queue_data_set_last(struct blk_mq_queue_data *obj,
					   bool value)
{
	obj->last = value;
}

/* blk_mq_tag_set accessors */

__nocfi const struct blk_mq_ops *rs_blk_mq_tag_set_ops(
		const struct blk_mq_tag_set *obj)
{
	return obj->ops;
}

__nocfi void rs_blk_mq_tag_set_set_ops(struct blk_mq_tag_set *obj,
				       const struct blk_mq_ops *value)
{
	obj->ops = value;
}

__nocfi u32 rs_blk_mq_tag_set_nr_maps(const struct blk_mq_tag_set *obj)
{
	return obj->nr_maps;
}

__nocfi void rs_blk_mq_tag_set_set_nr_maps(struct blk_mq_tag_set *obj,
					   u32 value)
{
	obj->nr_maps = value;
}

__nocfi u32 rs_blk_mq_tag_set_nr_hw_queues(const struct blk_mq_tag_set *obj)
{
	return obj->nr_hw_queues;
}

__nocfi void rs_blk_mq_tag_set_set_nr_hw_queues(struct blk_mq_tag_set *obj,
						u32 value)
{
	obj->nr_hw_queues = value;
}

__nocfi u32 rs_blk_mq_tag_set_queue_depth(const struct blk_mq_tag_set *obj)
{
	return obj->queue_depth;
}

__nocfi void rs_blk_mq_tag_set_set_queue_depth(struct blk_mq_tag_set *obj,
					       u32 value)
{
	obj->queue_depth = value;
}

__nocfi u32 rs_blk_mq_tag_set_reserved_tags(const struct blk_mq_tag_set *obj)
{
	return obj->reserved_tags;
}

__nocfi void rs_blk_mq_tag_set_set_reserved_tags(struct blk_mq_tag_set *obj,
						 u32 value)
{
	obj->reserved_tags = value;
}

__nocfi u32 rs_blk_mq_tag_set_cmd_size(const struct blk_mq_tag_set *obj)
{
	return obj->cmd_size;
}

__nocfi void rs_blk_mq_tag_set_set_cmd_size(struct blk_mq_tag_set *obj,
					    u32 value)
{
	obj->cmd_size = value;
}

__nocfi int rs_blk_mq_tag_set_numa_node(const struct blk_mq_tag_set *obj)
{
	return obj->numa_node;
}

__nocfi void rs_blk_mq_tag_set_set_numa_node(struct blk_mq_tag_set *obj,
					     int value)
{
	obj->numa_node = value;
}

__nocfi u32 rs_blk_mq_tag_set_timeout(const struct blk_mq_tag_set *obj)
{
	return obj->timeout;
}

__nocfi void rs_blk_mq_tag_set_set_timeout(struct blk_mq_tag_set *obj,
					   u32 value)
{
	obj->timeout = value;
}

__nocfi u32 rs_blk_mq_tag_set_flags(const struct blk_mq_tag_set *obj)
{
	return obj->flags;
}

__nocfi void rs_blk_mq_tag_set_set_flags(struct blk_mq_tag_set *obj,
					 u32 value)
{
	obj->flags = value;
}

__nocfi void *rs_blk_mq_tag_set_driver_data(const struct blk_mq_tag_set *obj)
{
	return obj->driver_data;
}

__nocfi void rs_blk_mq_tag_set_set_driver_data(struct blk_mq_tag_set *obj,
					       void *value)
{
	obj->driver_data = value;
}

/* blk_mq_ops accessors */

__nocfi rs_blk_mq_queue_rq_callback_t rs_blk_mq_ops_queue_rq(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_queue_rq_callback_t, obj->queue_rq);
}

__nocfi void rs_blk_mq_ops_set_queue_rq(struct blk_mq_ops *obj,
					rs_blk_mq_queue_rq_callback_t value)
{
	rs_fn_assign(obj->queue_rq, value);
}

__nocfi rs_blk_mq_void_callback_t rs_blk_mq_ops_queue_rqs(
		const struct blk_mq_ops *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	return rs_fn_to(rs_blk_mq_void_callback_t, obj->queue_rqs);
#else
	return NULL;
#endif
}

__nocfi void rs_blk_mq_ops_set_queue_rqs(struct blk_mq_ops *obj,
					 rs_blk_mq_void_callback_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	rs_fn_assign(obj->queue_rqs, value);
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi rs_blk_mq_commit_rqs_callback_t rs_blk_mq_ops_commit_rqs(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_commit_rqs_callback_t, obj->commit_rqs);
}

__nocfi void rs_blk_mq_ops_set_commit_rqs(struct blk_mq_ops *obj,
					  rs_blk_mq_commit_rqs_callback_t value)
{
	rs_fn_assign(obj->commit_rqs, value);
}

__nocfi rs_blk_mq_get_budget_callback_t rs_blk_mq_ops_get_budget(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_get_budget_callback_t, obj->get_budget);
}

__nocfi void rs_blk_mq_ops_set_get_budget(struct blk_mq_ops *obj,
					  rs_blk_mq_get_budget_callback_t value)
{
	rs_fn_assign(obj->get_budget, value);
}

__nocfi rs_blk_mq_put_budget_callback_t rs_blk_mq_ops_put_budget(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_put_budget_callback_t, obj->put_budget);
}

__nocfi void rs_blk_mq_ops_set_put_budget(struct blk_mq_ops *obj,
					  rs_blk_mq_put_budget_callback_t value)
{
	rs_fn_assign(obj->put_budget, value);
}

__nocfi rs_blk_mq_set_rq_budget_token_callback_t
rs_blk_mq_ops_set_rq_budget_token(const struct blk_mq_ops *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	return rs_fn_to(rs_blk_mq_set_rq_budget_token_callback_t, obj->set_rq_budget_token);
#else
	return NULL;
#endif
}

__nocfi void rs_blk_mq_ops_set_set_rq_budget_token(
		struct blk_mq_ops *obj,
		rs_blk_mq_set_rq_budget_token_callback_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	rs_fn_assign(obj->set_rq_budget_token, value);
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi rs_blk_mq_get_rq_budget_token_callback_t
rs_blk_mq_ops_get_rq_budget_token(const struct blk_mq_ops *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	return rs_fn_to(rs_blk_mq_get_rq_budget_token_callback_t, obj->get_rq_budget_token);
#else
	return NULL;
#endif
}

__nocfi void rs_blk_mq_ops_set_get_rq_budget_token(
		struct blk_mq_ops *obj,
		rs_blk_mq_get_rq_budget_token_callback_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	rs_fn_assign(obj->get_rq_budget_token, value);
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi rs_blk_mq_timeout_callback_t rs_blk_mq_ops_timeout(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_timeout_callback_t, obj->timeout);
}

__nocfi void rs_blk_mq_ops_set_timeout(struct blk_mq_ops *obj,
				       rs_blk_mq_timeout_callback_t value)
{
	rs_fn_assign(obj->timeout, value);
}

__nocfi rs_blk_mq_poll_callback_t rs_blk_mq_ops_poll(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_poll_callback_t, obj->poll);
}

__nocfi void rs_blk_mq_ops_set_poll(struct blk_mq_ops *obj,
				    rs_blk_mq_poll_callback_t value)
{
	rs_fn_assign(obj->poll, value);
}

__nocfi rs_blk_mq_complete_callback_t rs_blk_mq_ops_complete(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_complete_callback_t, obj->complete);
}

__nocfi void rs_blk_mq_ops_set_complete(struct blk_mq_ops *obj,
					rs_blk_mq_complete_callback_t value)
{
	rs_fn_assign(obj->complete, value);
}

__nocfi rs_blk_mq_init_hctx_callback_t rs_blk_mq_ops_init_hctx(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_init_hctx_callback_t, obj->init_hctx);
}

__nocfi void rs_blk_mq_ops_set_init_hctx(struct blk_mq_ops *obj,
					 rs_blk_mq_init_hctx_callback_t value)
{
	rs_fn_assign(obj->init_hctx, value);
}

__nocfi rs_blk_mq_exit_hctx_callback_t rs_blk_mq_ops_exit_hctx(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_exit_hctx_callback_t, obj->exit_hctx);
}

__nocfi void rs_blk_mq_ops_set_exit_hctx(struct blk_mq_ops *obj,
					 rs_blk_mq_exit_hctx_callback_t value)
{
	rs_fn_assign(obj->exit_hctx, value);
}

__nocfi rs_blk_mq_init_request_callback_t rs_blk_mq_ops_init_request(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_init_request_callback_t, obj->init_request);
}

__nocfi void rs_blk_mq_ops_set_init_request(struct blk_mq_ops *obj,
					    rs_blk_mq_init_request_callback_t value)
{
	rs_fn_assign(obj->init_request, value);
}

__nocfi rs_blk_mq_exit_request_callback_t rs_blk_mq_ops_exit_request(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_exit_request_callback_t, obj->exit_request);
}

__nocfi void rs_blk_mq_ops_set_exit_request(struct blk_mq_ops *obj,
					    rs_blk_mq_exit_request_callback_t value)
{
	rs_fn_assign(obj->exit_request, value);
}

__nocfi rs_blk_mq_cleanup_rq_callback_t rs_blk_mq_ops_cleanup_rq(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_cleanup_rq_callback_t, obj->cleanup_rq);
}

__nocfi void rs_blk_mq_ops_set_cleanup_rq(struct blk_mq_ops *obj,
					  rs_blk_mq_cleanup_rq_callback_t value)
{
	rs_fn_assign(obj->cleanup_rq, value);
}

__nocfi rs_blk_mq_busy_callback_t rs_blk_mq_ops_busy(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_busy_callback_t, obj->busy);
}

__nocfi void rs_blk_mq_ops_set_busy(struct blk_mq_ops *obj,
				    rs_blk_mq_busy_callback_t value)
{
	rs_fn_assign(obj->busy, value);
}

__nocfi rs_blk_mq_map_queues_callback_t rs_blk_mq_ops_map_queues(
		const struct blk_mq_ops *obj)
{
	return rs_fn_to(rs_blk_mq_map_queues_callback_t, obj->map_queues);
}

__nocfi void rs_blk_mq_ops_set_map_queues(struct blk_mq_ops *obj,
					  rs_blk_mq_map_queues_callback_t value)
{
	rs_fn_assign(obj->map_queues, value);
}

__nocfi rs_blk_mq_show_rq_callback_t rs_blk_mq_ops_show_rq(
		const struct blk_mq_ops *obj)
{
#ifdef CONFIG_BLK_DEBUG_FS
	return rs_fn_to(rs_blk_mq_show_rq_callback_t, obj->show_rq);
#else
	return NULL;
#endif
}

__nocfi void rs_blk_mq_ops_set_show_rq(struct blk_mq_ops *obj,
				       rs_blk_mq_show_rq_callback_t value)
{
#ifdef CONFIG_BLK_DEBUG_FS
	rs_fn_assign(obj->show_rq, value);
#else
	(void)obj;
	(void)value;
#endif
}

/* block_device_operations accessors */

__nocfi rs_bdo_callback_t rs_block_device_operations_submit_bio(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->submit_bio);
}

__nocfi void rs_block_device_operations_set_submit_bio(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->submit_bio, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_open(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->open);
}

__nocfi void rs_block_device_operations_set_open(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->open, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_release(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->release);
}

__nocfi void rs_block_device_operations_set_release(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->release, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_ioctl(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->ioctl);
}

__nocfi void rs_block_device_operations_set_ioctl(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->ioctl, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_compat_ioctl(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->compat_ioctl);
}

__nocfi void rs_block_device_operations_set_compat_ioctl(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->compat_ioctl, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_check_events(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->check_events);
}

__nocfi void rs_block_device_operations_set_check_events(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->check_events, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_unlock_native_capacity(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->unlock_native_capacity);
}

__nocfi void rs_block_device_operations_set_unlock_native_capacity(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->unlock_native_capacity, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_getgeo(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->getgeo);
}

__nocfi void rs_block_device_operations_set_getgeo(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->getgeo, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_set_read_only(
		const struct block_device_operations *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	return rs_fn_to(rs_bdo_callback_t, obj->set_read_only);
#else
	return NULL;
#endif
}

__nocfi void rs_block_device_operations_set_set_read_only(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	rs_fn_assign(obj->set_read_only, value);
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi rs_bdo_callback_t rs_block_device_operations_swap_slot_free_notify(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->swap_slot_free_notify);
}

__nocfi void rs_block_device_operations_set_swap_slot_free_notify(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->swap_slot_free_notify, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_report_zones(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->report_zones);
}

__nocfi void rs_block_device_operations_set_report_zones(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->report_zones, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_devnode(
		const struct block_device_operations *obj)
{
	return rs_fn_to(rs_bdo_callback_t, obj->devnode);
}

__nocfi void rs_block_device_operations_set_devnode(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
	rs_fn_assign(obj->devnode, value);
}

__nocfi rs_bdo_callback_t rs_block_device_operations_alternative_gpt_sector(
		const struct block_device_operations *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	return rs_fn_to(rs_bdo_callback_t, obj->alternative_gpt_sector);
#else
	return NULL;
#endif
}

__nocfi void rs_block_device_operations_set_alternative_gpt_sector(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	rs_fn_assign(obj->alternative_gpt_sector, value);
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi rs_bdo_callback_t rs_block_device_operations_get_unique_id(
		const struct block_device_operations *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	return rs_fn_to(rs_bdo_callback_t, obj->get_unique_id);
#else
	return NULL;
#endif
}

__nocfi void rs_block_device_operations_set_get_unique_id(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	rs_fn_assign(obj->get_unique_id, value);
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi struct module *rs_block_device_operations_owner(
		const struct block_device_operations *obj)
{
	return obj->owner;
}

__nocfi void rs_block_device_operations_set_owner(
		struct block_device_operations *obj, struct module *value)
{
	obj->owner = value;
}

__nocfi void *rs_block_device_operations_pr_ops(
		const struct block_device_operations *obj)
{
	return (void *)obj->pr_ops;
}

__nocfi void rs_block_device_operations_set_pr_ops(
		struct block_device_operations *obj, void *value)
{
	obj->pr_ops = (const struct pr_ops *)value;
}

__nocfi rs_bdo_callback_t rs_block_device_operations_free_disk(
		const struct block_device_operations *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	return rs_fn_to(rs_bdo_callback_t, obj->free_disk);
#else
	return NULL;
#endif
}

__nocfi void rs_block_device_operations_set_free_disk(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	rs_fn_assign(obj->free_disk, value);
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi rs_bdo_callback_t rs_block_device_operations_poll_bio(
		const struct block_device_operations *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	return rs_fn_to(rs_bdo_callback_t, obj->poll_bio);
#else
	return NULL;
#endif
}

__nocfi void rs_block_device_operations_set_poll_bio(
		struct block_device_operations *obj, rs_bdo_callback_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	rs_fn_assign(obj->poll_bio, value);
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi const struct block_device_operations *rs_block_device_operations_zero(void)
{
	return &rs_bdo_zero;
}

/* gendisk accessors */

__nocfi const struct block_device_operations *rs_gendisk_fops(
		const struct gendisk *obj)
{
	return obj->fops;
}

__nocfi void rs_gendisk_set_fops(struct gendisk *obj,
				 const struct block_device_operations *value)
{
	obj->fops = value;
}

__nocfi char *rs_gendisk_disk_name(struct gendisk *obj)
{
	return obj->disk_name;
}

/* queue_limits accessors */

__nocfi rs_blk_features_t rs_queue_limits_features(
		const struct queue_limits *obj)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	return obj->features;
#else
	return 0;
#endif
}

__nocfi void rs_queue_limits_set_features(struct queue_limits *obj,
					  rs_blk_features_t value)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	obj->features = value;
#else
	(void)obj;
	(void)value;
#endif
}

__nocfi u32 rs_queue_limits_logical_block_size(const struct queue_limits *obj)
{
	return obj->logical_block_size;
}

__nocfi void rs_queue_limits_set_logical_block_size(struct queue_limits *obj,
						    u32 value)
{
	obj->logical_block_size = value;
}

__nocfi u32 rs_queue_limits_physical_block_size(const struct queue_limits *obj)
{
	return obj->physical_block_size;
}

__nocfi void rs_queue_limits_set_physical_block_size(struct queue_limits *obj,
						     u32 value)
{
	obj->physical_block_size = value;
}

EXPORT_SYMBOL_GPL(rs_blk_mq_alloc_disk_with_limits);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_alloc);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_alloc);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_free);
EXPORT_SYMBOL_GPL(rs_blk_mq_queue_data_rq);
EXPORT_SYMBOL_GPL(rs_blk_mq_queue_data_set_rq);
EXPORT_SYMBOL_GPL(rs_blk_mq_queue_data_last);
EXPORT_SYMBOL_GPL(rs_blk_mq_queue_data_set_last);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_ops);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_ops);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_nr_maps);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_nr_maps);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_nr_hw_queues);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_nr_hw_queues);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_queue_depth);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_queue_depth);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_reserved_tags);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_reserved_tags);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_cmd_size);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_cmd_size);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_numa_node);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_numa_node);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_timeout);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_timeout);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_flags);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_flags);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_driver_data);
EXPORT_SYMBOL_GPL(rs_blk_mq_tag_set_set_driver_data);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_queue_rq);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_queue_rq);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_queue_rqs);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_queue_rqs);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_commit_rqs);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_commit_rqs);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_get_budget);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_get_budget);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_put_budget);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_put_budget);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_rq_budget_token);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_set_rq_budget_token);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_get_rq_budget_token);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_get_rq_budget_token);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_timeout);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_timeout);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_poll);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_poll);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_complete);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_complete);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_init_hctx);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_init_hctx);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_exit_hctx);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_exit_hctx);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_init_request);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_init_request);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_exit_request);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_exit_request);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_cleanup_rq);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_cleanup_rq);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_busy);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_busy);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_map_queues);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_map_queues);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_show_rq);
EXPORT_SYMBOL_GPL(rs_blk_mq_ops_set_show_rq);
EXPORT_SYMBOL_GPL(rs_block_device_operations_submit_bio);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_submit_bio);
EXPORT_SYMBOL_GPL(rs_block_device_operations_open);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_open);
EXPORT_SYMBOL_GPL(rs_block_device_operations_release);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_release);
EXPORT_SYMBOL_GPL(rs_block_device_operations_ioctl);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_ioctl);
EXPORT_SYMBOL_GPL(rs_block_device_operations_compat_ioctl);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_compat_ioctl);
EXPORT_SYMBOL_GPL(rs_block_device_operations_check_events);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_check_events);
EXPORT_SYMBOL_GPL(rs_block_device_operations_unlock_native_capacity);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_unlock_native_capacity);
EXPORT_SYMBOL_GPL(rs_block_device_operations_getgeo);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_getgeo);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_read_only);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_set_read_only);
EXPORT_SYMBOL_GPL(rs_block_device_operations_swap_slot_free_notify);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_swap_slot_free_notify);
EXPORT_SYMBOL_GPL(rs_block_device_operations_report_zones);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_report_zones);
EXPORT_SYMBOL_GPL(rs_block_device_operations_devnode);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_devnode);
EXPORT_SYMBOL_GPL(rs_block_device_operations_alternative_gpt_sector);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_alternative_gpt_sector);
EXPORT_SYMBOL_GPL(rs_block_device_operations_get_unique_id);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_get_unique_id);
EXPORT_SYMBOL_GPL(rs_block_device_operations_owner);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_owner);
EXPORT_SYMBOL_GPL(rs_block_device_operations_pr_ops);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_pr_ops);
EXPORT_SYMBOL_GPL(rs_block_device_operations_free_disk);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_free_disk);
EXPORT_SYMBOL_GPL(rs_block_device_operations_poll_bio);
EXPORT_SYMBOL_GPL(rs_block_device_operations_set_poll_bio);
EXPORT_SYMBOL_GPL(rs_block_device_operations_zero);
EXPORT_SYMBOL_GPL(rs_gendisk_fops);
EXPORT_SYMBOL_GPL(rs_gendisk_set_fops);
EXPORT_SYMBOL_GPL(rs_gendisk_disk_name);
EXPORT_SYMBOL_GPL(rs_queue_limits_features);
EXPORT_SYMBOL_GPL(rs_queue_limits_set_features);
EXPORT_SYMBOL_GPL(rs_queue_limits_logical_block_size);
EXPORT_SYMBOL_GPL(rs_queue_limits_set_logical_block_size);
EXPORT_SYMBOL_GPL(rs_queue_limits_physical_block_size);
EXPORT_SYMBOL_GPL(rs_queue_limits_set_physical_block_size);
