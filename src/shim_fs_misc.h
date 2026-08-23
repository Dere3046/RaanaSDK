// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#ifndef RUST_SUPPORT_SHIM_FS_MISC_H
#define RUST_SUPPORT_SHIM_FS_MISC_H

#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/firmware.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/types.h>

struct module;

typedef loff_t (*rs_llseek_t)(struct file *, loff_t, int);
typedef ssize_t (*rs_read_iter_t)(struct kiocb *, struct iov_iter *);
typedef long (*rs_ioctl_t)(struct file *, unsigned int, unsigned long);
typedef int (*rs_mmap_t)(struct file *, struct vm_area_struct *);
typedef int (*rs_open_t)(struct inode *, struct file *);
typedef void (*rs_show_fdinfo_t)(struct seq_file *, struct file *);

/*
 * Layout-compatible with rust::miscdevice::MiscdeviceFops (repr(C)).
 * The C side copies these callbacks into a real struct file_operations.
 */
struct rs_miscdevice_fops {
	rs_open_t open;
	rs_open_t release;
	rs_mmap_t mmap;
	rs_llseek_t llseek;
	rs_read_iter_t read_iter;
	rs_read_iter_t write_iter;
	rs_ioctl_t unlocked_ioctl;
#ifdef CONFIG_COMPAT
	rs_ioctl_t compat_ioctl;
#endif
	rs_show_fdinfo_t show_fdinfo;
};

/* Time */
unsigned long rs___msecs_to_jiffies(unsigned int msecs);
ktime_t rs_ktime_get(void);

/* File and fd table */
struct file *rs_fget(unsigned int fd);
void rs_fput(struct file *file);
const struct cred *rs_file_cred(struct file *file);
unsigned int rs_file_flags(struct file *file);
int rs_get_unused_fd_flags(unsigned int flags);
void rs_put_unused_fd(unsigned int fd);
void rs_fd_install(unsigned int fd, struct file *file);

/* Firmware accessors */
size_t rs_firmware_size(const struct firmware *obj);
const u8 *rs_firmware_data(const struct firmware *obj);

/* miscdevice construction / destruction */
struct miscdevice *rs_miscdevice_init(const char *name,
				      const struct rs_miscdevice_fops *fops,
				      void *owner);
void rs_miscdevice_free(struct miscdevice *misc);

/* miscdevice accessors */
int rs_miscdevice_minor(const struct miscdevice *obj);
void rs_miscdevice_set_minor(struct miscdevice *obj, int value);
const char *rs_miscdevice_name(const struct miscdevice *obj);
void rs_miscdevice_set_name(struct miscdevice *obj, const char *value);
const struct file_operations *rs_miscdevice_fops(const struct miscdevice *obj);
void rs_miscdevice_set_fops(struct miscdevice *obj,
			    const struct file_operations *value);
struct device *rs_miscdevice_this_device(const struct miscdevice *obj);
void rs_miscdevice_set_this_device(struct miscdevice *obj,
				   struct device *value);
void *rs_miscdevice_owner(const struct miscdevice *obj);

/* file_operations accessors */
struct module *rs_file_operations_owner(const struct file_operations *obj);
void rs_file_operations_set_owner(struct file_operations *obj,
				  struct module *value);
rs_llseek_t rs_file_operations_llseek(const struct file_operations *obj);
void rs_file_operations_set_llseek(struct file_operations *obj,
				   rs_llseek_t value);
rs_read_iter_t rs_file_operations_read_iter(const struct file_operations *obj);
void rs_file_operations_set_read_iter(struct file_operations *obj,
				      rs_read_iter_t value);
rs_read_iter_t rs_file_operations_write_iter(const struct file_operations *obj);
void rs_file_operations_set_write_iter(struct file_operations *obj,
				       rs_read_iter_t value);
rs_ioctl_t rs_file_operations_unlocked_ioctl(const struct file_operations *obj);
void rs_file_operations_set_unlocked_ioctl(struct file_operations *obj,
					   rs_ioctl_t value);
#ifdef CONFIG_COMPAT
rs_ioctl_t rs_file_operations_compat_ioctl(const struct file_operations *obj);
void rs_file_operations_set_compat_ioctl(struct file_operations *obj,
					 rs_ioctl_t value);
#endif
rs_mmap_t rs_file_operations_mmap(const struct file_operations *obj);
void rs_file_operations_set_mmap(struct file_operations *obj,
				 rs_mmap_t value);
rs_open_t rs_file_operations_open(const struct file_operations *obj);
void rs_file_operations_set_open(struct file_operations *obj,
				 rs_open_t value);
rs_open_t rs_file_operations_release(const struct file_operations *obj);
void rs_file_operations_set_release(struct file_operations *obj,
				    rs_open_t value);
rs_show_fdinfo_t rs_file_operations_show_fdinfo(const struct file_operations *obj);
void rs_file_operations_set_show_fdinfo(struct file_operations *obj,
					rs_show_fdinfo_t value);

struct file *rs_kiocb_ki_filp(const struct kiocb *kiocb);
loff_t rs_kiocb_ki_pos(const struct kiocb *kiocb);
void rs_kiocb_set_ki_pos(struct kiocb *kiocb, loff_t value);

#endif /* RUST_SUPPORT_SHIM_FS_MISC_H */
