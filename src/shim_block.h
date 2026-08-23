// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#ifndef RUST_SUPPORT_SHIM_BLOCK_H
#define RUST_SUPPORT_SHIM_BLOCK_H

#include <linux/types.h>
#include <linux/version.h>
#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0)
#include <linux/genhd.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
typedef blk_features_t rs_blk_features_t;
#else
typedef u32 rs_blk_features_t;
#endif

struct io_comp_batch;

/*
 * The Rust bindings intentionally expose block_device_operations callbacks as
 * opaque one-argument callbacks.  Keep the C side ABI-compatible with that.
 */
typedef void (*rs_bdo_callback_t)(void *);

/* blk-mq callback typedefs matching rust/src/bindings.rs */
typedef blk_status_t (*rs_blk_mq_queue_rq_callback_t)(
		struct blk_mq_hw_ctx *, const struct blk_mq_queue_data *);
typedef void (*rs_blk_mq_void_callback_t)(void *);
typedef void (*rs_blk_mq_commit_rqs_callback_t)(struct blk_mq_hw_ctx *);
typedef int (*rs_blk_mq_get_budget_callback_t)(void *);
typedef void (*rs_blk_mq_put_budget_callback_t)(void *, int);
typedef void (*rs_blk_mq_set_rq_budget_token_callback_t)(struct request *, int);
typedef int (*rs_blk_mq_get_rq_budget_token_callback_t)(struct request *);
typedef int (*rs_blk_mq_timeout_callback_t)(struct request *);
typedef int (*rs_blk_mq_poll_callback_t)(struct blk_mq_hw_ctx *,
					 struct io_comp_batch *);
typedef void (*rs_blk_mq_complete_callback_t)(struct request *);
typedef int (*rs_blk_mq_init_hctx_callback_t)(struct blk_mq_hw_ctx *, void *,
					      unsigned int);
typedef void (*rs_blk_mq_exit_hctx_callback_t)(struct blk_mq_hw_ctx *,
					       unsigned int);
typedef int (*rs_blk_mq_init_request_callback_t)(struct blk_mq_tag_set *,
						 struct request *,
						 unsigned int, unsigned int);
typedef void (*rs_blk_mq_exit_request_callback_t)(struct blk_mq_tag_set *,
						   struct request *,
						   unsigned int);
typedef void (*rs_blk_mq_cleanup_rq_callback_t)(struct request *);
typedef bool (*rs_blk_mq_busy_callback_t)(void *);
typedef void (*rs_blk_mq_map_queues_callback_t)(struct blk_mq_tag_set *);
typedef void (*rs_blk_mq_show_rq_callback_t)(struct seq_file *,
					     struct request *);

struct gendisk *rs_blk_mq_alloc_disk_with_limits(struct blk_mq_tag_set *tag_set,
						 bool rotational,
						 u32 logical_block_size,
						 u32 physical_block_size,
						 void *queuedata,
						 struct lock_class_key *lkclass);
const struct blk_mq_ops *rs_blk_mq_ops_alloc(
		rs_blk_mq_queue_rq_callback_t queue_rq,
		rs_blk_mq_commit_rqs_callback_t commit_rqs,
		rs_blk_mq_poll_callback_t poll,
		rs_blk_mq_complete_callback_t complete,
		rs_blk_mq_init_hctx_callback_t init_hctx,
		rs_blk_mq_exit_hctx_callback_t exit_hctx,
		rs_blk_mq_init_request_callback_t init_request,
		rs_blk_mq_exit_request_callback_t exit_request);

struct blk_mq_tag_set *rs_blk_mq_tag_set_alloc(
		const struct blk_mq_ops *ops, u32 nr_hw_queues, u32 num_tags,
		u32 num_maps, u32 cmd_size, u32 flags, int numa_node,
		u32 timeout, void *driver_data);
void rs_blk_mq_tag_set_free(struct blk_mq_tag_set *set);

/* blk_mq_queue_data accessors */
struct request *rs_blk_mq_queue_data_rq(const struct blk_mq_queue_data *obj);
void rs_blk_mq_queue_data_set_rq(struct blk_mq_queue_data *obj,
				 struct request *value);
bool rs_blk_mq_queue_data_last(const struct blk_mq_queue_data *obj);
void rs_blk_mq_queue_data_set_last(struct blk_mq_queue_data *obj, bool value);

/* blk_mq_tag_set accessors */
const struct blk_mq_ops *rs_blk_mq_tag_set_ops(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_ops(struct blk_mq_tag_set *obj,
			       const struct blk_mq_ops *value);
u32 rs_blk_mq_tag_set_nr_maps(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_nr_maps(struct blk_mq_tag_set *obj, u32 value);
u32 rs_blk_mq_tag_set_nr_hw_queues(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_nr_hw_queues(struct blk_mq_tag_set *obj, u32 value);
u32 rs_blk_mq_tag_set_queue_depth(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_queue_depth(struct blk_mq_tag_set *obj, u32 value);
u32 rs_blk_mq_tag_set_reserved_tags(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_reserved_tags(struct blk_mq_tag_set *obj, u32 value);
u32 rs_blk_mq_tag_set_cmd_size(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_cmd_size(struct blk_mq_tag_set *obj, u32 value);
int rs_blk_mq_tag_set_numa_node(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_numa_node(struct blk_mq_tag_set *obj, int value);
u32 rs_blk_mq_tag_set_timeout(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_timeout(struct blk_mq_tag_set *obj, u32 value);
u32 rs_blk_mq_tag_set_flags(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_flags(struct blk_mq_tag_set *obj, u32 value);
void *rs_blk_mq_tag_set_driver_data(const struct blk_mq_tag_set *obj);
void rs_blk_mq_tag_set_set_driver_data(struct blk_mq_tag_set *obj, void *value);

/* blk_mq_ops accessors */
rs_blk_mq_queue_rq_callback_t rs_blk_mq_ops_queue_rq(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_queue_rq(struct blk_mq_ops *obj,
				rs_blk_mq_queue_rq_callback_t value);
rs_blk_mq_void_callback_t rs_blk_mq_ops_queue_rqs(const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_queue_rqs(struct blk_mq_ops *obj,
				 rs_blk_mq_void_callback_t value);
rs_blk_mq_commit_rqs_callback_t rs_blk_mq_ops_commit_rqs(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_commit_rqs(struct blk_mq_ops *obj,
				  rs_blk_mq_commit_rqs_callback_t value);
rs_blk_mq_get_budget_callback_t rs_blk_mq_ops_get_budget(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_get_budget(struct blk_mq_ops *obj,
				  rs_blk_mq_get_budget_callback_t value);
rs_blk_mq_put_budget_callback_t rs_blk_mq_ops_put_budget(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_put_budget(struct blk_mq_ops *obj,
				  rs_blk_mq_put_budget_callback_t value);
rs_blk_mq_set_rq_budget_token_callback_t rs_blk_mq_ops_set_rq_budget_token(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_set_rq_budget_token(struct blk_mq_ops *obj,
		rs_blk_mq_set_rq_budget_token_callback_t value);
rs_blk_mq_get_rq_budget_token_callback_t rs_blk_mq_ops_get_rq_budget_token(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_get_rq_budget_token(struct blk_mq_ops *obj,
		rs_blk_mq_get_rq_budget_token_callback_t value);
rs_blk_mq_timeout_callback_t rs_blk_mq_ops_timeout(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_timeout(struct blk_mq_ops *obj,
			       rs_blk_mq_timeout_callback_t value);
rs_blk_mq_poll_callback_t rs_blk_mq_ops_poll(const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_poll(struct blk_mq_ops *obj,
			    rs_blk_mq_poll_callback_t value);
rs_blk_mq_complete_callback_t rs_blk_mq_ops_complete(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_complete(struct blk_mq_ops *obj,
				rs_blk_mq_complete_callback_t value);
rs_blk_mq_init_hctx_callback_t rs_blk_mq_ops_init_hctx(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_init_hctx(struct blk_mq_ops *obj,
				 rs_blk_mq_init_hctx_callback_t value);
rs_blk_mq_exit_hctx_callback_t rs_blk_mq_ops_exit_hctx(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_exit_hctx(struct blk_mq_ops *obj,
				 rs_blk_mq_exit_hctx_callback_t value);
rs_blk_mq_init_request_callback_t rs_blk_mq_ops_init_request(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_init_request(struct blk_mq_ops *obj,
				    rs_blk_mq_init_request_callback_t value);
rs_blk_mq_exit_request_callback_t rs_blk_mq_ops_exit_request(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_exit_request(struct blk_mq_ops *obj,
				    rs_blk_mq_exit_request_callback_t value);
rs_blk_mq_cleanup_rq_callback_t rs_blk_mq_ops_cleanup_rq(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_cleanup_rq(struct blk_mq_ops *obj,
				  rs_blk_mq_cleanup_rq_callback_t value);
rs_blk_mq_busy_callback_t rs_blk_mq_ops_busy(const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_busy(struct blk_mq_ops *obj,
			    rs_blk_mq_busy_callback_t value);
rs_blk_mq_map_queues_callback_t rs_blk_mq_ops_map_queues(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_map_queues(struct blk_mq_ops *obj,
				  rs_blk_mq_map_queues_callback_t value);
rs_blk_mq_show_rq_callback_t rs_blk_mq_ops_show_rq(
		const struct blk_mq_ops *obj);
void rs_blk_mq_ops_set_show_rq(struct blk_mq_ops *obj,
			       rs_blk_mq_show_rq_callback_t value);

/* block_device_operations accessors */
rs_bdo_callback_t rs_block_device_operations_submit_bio(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_submit_bio(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_open(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_open(struct block_device_operations *obj,
					 rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_release(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_release(struct block_device_operations *obj,
					    rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_ioctl(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_ioctl(struct block_device_operations *obj,
					  rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_compat_ioctl(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_compat_ioctl(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_check_events(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_check_events(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_unlock_native_capacity(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_unlock_native_capacity(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_getgeo(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_getgeo(struct block_device_operations *obj,
					   rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_set_read_only(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_set_read_only(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_swap_slot_free_notify(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_swap_slot_free_notify(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_report_zones(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_report_zones(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_devnode(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_devnode(struct block_device_operations *obj,
					    rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_alternative_gpt_sector(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_alternative_gpt_sector(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_get_unique_id(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_get_unique_id(
		struct block_device_operations *obj, rs_bdo_callback_t value);
struct module *rs_block_device_operations_owner(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_owner(struct block_device_operations *obj,
					  struct module *value);
void *rs_block_device_operations_pr_ops(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_pr_ops(struct block_device_operations *obj,
					   void *value);
rs_bdo_callback_t rs_block_device_operations_free_disk(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_free_disk(
		struct block_device_operations *obj, rs_bdo_callback_t value);
rs_bdo_callback_t rs_block_device_operations_poll_bio(
		const struct block_device_operations *obj);
void rs_block_device_operations_set_poll_bio(
		struct block_device_operations *obj, rs_bdo_callback_t value);
const struct block_device_operations *rs_block_device_operations_zero(void);

/* gendisk accessors */
const struct block_device_operations *rs_gendisk_fops(
		const struct gendisk *obj);
void rs_gendisk_set_fops(struct gendisk *obj,
			 const struct block_device_operations *value);
char *rs_gendisk_disk_name(struct gendisk *obj);

/* queue_limits accessors */
rs_blk_features_t rs_queue_limits_features(const struct queue_limits *obj);
void rs_queue_limits_set_features(struct queue_limits *obj,
				  rs_blk_features_t value);
u32 rs_queue_limits_logical_block_size(const struct queue_limits *obj);
void rs_queue_limits_set_logical_block_size(struct queue_limits *obj,
					    u32 value);
u32 rs_queue_limits_physical_block_size(const struct queue_limits *obj);
void rs_queue_limits_set_physical_block_size(struct queue_limits *obj,
					     u32 value);

#endif /* RUST_SUPPORT_SHIM_BLOCK_H */
