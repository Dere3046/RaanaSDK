// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#include <linux/cred.h>
#include <linux/export.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/firmware.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/timekeeping.h>
#include <linux/types.h>

#include "shim_fs_misc.h"

struct rs_miscdevice_priv {
	struct miscdevice misc;
	struct file_operations fops;
	void *owner;
};

__nocfi unsigned long rs___msecs_to_jiffies(unsigned int msecs)
{
	return __msecs_to_jiffies(msecs);
}

__nocfi ktime_t rs_ktime_get(void)
{
	return ktime_get();
}

__nocfi struct file *rs_fget(unsigned int fd)
{
	return fget(fd);
}

__nocfi void rs_fput(struct file *file)
{
	fput(file);
}

__nocfi const struct cred *rs_file_cred(struct file *file)
{
	return file->f_cred;
}

__nocfi unsigned int rs_file_flags(struct file *file)
{
	return file->f_flags;
}

__nocfi int rs_get_unused_fd_flags(unsigned int flags)
{
	return get_unused_fd_flags(flags);
}

__nocfi void rs_put_unused_fd(unsigned int fd)
{
	put_unused_fd(fd);
}

__nocfi void rs_fd_install(unsigned int fd, struct file *file)
{
	fd_install(fd, file);
}

__nocfi size_t rs_firmware_size(const struct firmware *obj)
{
	return obj->size;
}

__nocfi const u8 *rs_firmware_data(const struct firmware *obj)
{
	return obj->data;
}

__nocfi struct miscdevice *rs_miscdevice_init(const char *name,
					      const struct rs_miscdevice_fops *fops,
					      void *owner)
{
	struct rs_miscdevice_priv *priv;

	if (!name || !fops)
		return NULL;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return NULL;

	priv->owner = owner;
	priv->misc.minor = MISC_DYNAMIC_MINOR;
	priv->misc.name = name;
	priv->misc.fops = &priv->fops;

	/* fops.owner is left NULL (kzalloc zeroed), matching upstream Rust. */
	priv->fops.open = fops->open;
	priv->fops.release = fops->release;
	priv->fops.mmap = fops->mmap;
	priv->fops.llseek = fops->llseek;
	priv->fops.read_iter = fops->read_iter;
	priv->fops.write_iter = fops->write_iter;
	priv->fops.unlocked_ioctl = fops->unlocked_ioctl;
#ifdef CONFIG_COMPAT
	priv->fops.compat_ioctl = fops->compat_ioctl;
#endif
	priv->fops.show_fdinfo = fops->show_fdinfo;

	return &priv->misc;
}

__nocfi void rs_miscdevice_free(struct miscdevice *misc)
{
	struct rs_miscdevice_priv *priv;

	if (!misc)
		return;

	priv = container_of(misc, struct rs_miscdevice_priv, misc);
	kfree(priv);
}

__nocfi int rs_miscdevice_minor(const struct miscdevice *obj)
{
	return obj->minor;
}

__nocfi void rs_miscdevice_set_minor(struct miscdevice *obj, int value)
{
	obj->minor = value;
}

__nocfi const char *rs_miscdevice_name(const struct miscdevice *obj)
{
	return obj->name;
}

__nocfi void rs_miscdevice_set_name(struct miscdevice *obj, const char *value)
{
	obj->name = value;
}

__nocfi const struct file_operations *rs_miscdevice_fops(const struct miscdevice *obj)
{
	return obj->fops;
}

__nocfi void rs_miscdevice_set_fops(struct miscdevice *obj,
				    const struct file_operations *value)
{
	obj->fops = value;
}

__nocfi struct device *rs_miscdevice_this_device(const struct miscdevice *obj)
{
	return obj->this_device;
}

__nocfi void rs_miscdevice_set_this_device(struct miscdevice *obj,
					   struct device *value)
{
	obj->this_device = value;
}

__nocfi void *rs_miscdevice_owner(const struct miscdevice *obj)
{
	struct rs_miscdevice_priv *priv;

	if (!obj)
		return NULL;

	priv = container_of((struct miscdevice *)obj,
			    struct rs_miscdevice_priv, misc);
	return priv->owner;
}

__nocfi struct module *rs_file_operations_owner(const struct file_operations *obj)
{
	return obj->owner;
}

__nocfi void rs_file_operations_set_owner(struct file_operations *obj,
					  struct module *value)
{
	obj->owner = value;
}

__nocfi rs_llseek_t rs_file_operations_llseek(const struct file_operations *obj)
{
	return obj->llseek;
}

__nocfi void rs_file_operations_set_llseek(struct file_operations *obj,
					   rs_llseek_t value)
{
	obj->llseek = value;
}

__nocfi rs_read_iter_t rs_file_operations_read_iter(const struct file_operations *obj)
{
	return obj->read_iter;
}

__nocfi void rs_file_operations_set_read_iter(struct file_operations *obj,
					      rs_read_iter_t value)
{
	obj->read_iter = value;
}

__nocfi rs_read_iter_t rs_file_operations_write_iter(const struct file_operations *obj)
{
	return obj->write_iter;
}

__nocfi void rs_file_operations_set_write_iter(struct file_operations *obj,
					       rs_read_iter_t value)
{
	obj->write_iter = value;
}

__nocfi rs_ioctl_t rs_file_operations_unlocked_ioctl(const struct file_operations *obj)
{
	return obj->unlocked_ioctl;
}

__nocfi void rs_file_operations_set_unlocked_ioctl(struct file_operations *obj,
						   rs_ioctl_t value)
{
	obj->unlocked_ioctl = value;
}

#ifdef CONFIG_COMPAT
__nocfi rs_ioctl_t rs_file_operations_compat_ioctl(const struct file_operations *obj)
{
	return obj->compat_ioctl;
}

__nocfi void rs_file_operations_set_compat_ioctl(struct file_operations *obj,
						 rs_ioctl_t value)
{
	obj->compat_ioctl = value;
}
#endif

__nocfi rs_mmap_t rs_file_operations_mmap(const struct file_operations *obj)
{
	return obj->mmap;
}

__nocfi void rs_file_operations_set_mmap(struct file_operations *obj,
					 rs_mmap_t value)
{
	obj->mmap = value;
}

__nocfi rs_open_t rs_file_operations_open(const struct file_operations *obj)
{
	return obj->open;
}

__nocfi void rs_file_operations_set_open(struct file_operations *obj,
					 rs_open_t value)
{
	obj->open = value;
}

__nocfi rs_open_t rs_file_operations_release(const struct file_operations *obj)
{
	return obj->release;
}

__nocfi void rs_file_operations_set_release(struct file_operations *obj,
					    rs_open_t value)
{
	obj->release = value;
}

__nocfi rs_show_fdinfo_t rs_file_operations_show_fdinfo(const struct file_operations *obj)
{
	return obj->show_fdinfo;
}

__nocfi void rs_file_operations_set_show_fdinfo(struct file_operations *obj,
						rs_show_fdinfo_t value)
{
	obj->show_fdinfo = value;
}

EXPORT_SYMBOL_GPL(rs___msecs_to_jiffies);
EXPORT_SYMBOL_GPL(rs_ktime_get);
EXPORT_SYMBOL_GPL(rs_fget);
EXPORT_SYMBOL_GPL(rs_fput);
EXPORT_SYMBOL_GPL(rs_file_cred);
EXPORT_SYMBOL_GPL(rs_file_flags);
EXPORT_SYMBOL_GPL(rs_get_unused_fd_flags);
EXPORT_SYMBOL_GPL(rs_put_unused_fd);
EXPORT_SYMBOL_GPL(rs_fd_install);
EXPORT_SYMBOL_GPL(rs_firmware_size);
EXPORT_SYMBOL_GPL(rs_firmware_data);
EXPORT_SYMBOL_GPL(rs_miscdevice_init);
EXPORT_SYMBOL_GPL(rs_miscdevice_free);
EXPORT_SYMBOL_GPL(rs_miscdevice_minor);
EXPORT_SYMBOL_GPL(rs_miscdevice_name);
EXPORT_SYMBOL_GPL(rs_miscdevice_fops);
EXPORT_SYMBOL_GPL(rs_miscdevice_owner);
EXPORT_SYMBOL_GPL(rs_miscdevice_this_device);
EXPORT_SYMBOL_GPL(rs_miscdevice_set_minor);
EXPORT_SYMBOL_GPL(rs_miscdevice_set_name);
EXPORT_SYMBOL_GPL(rs_miscdevice_set_fops);
EXPORT_SYMBOL_GPL(rs_miscdevice_set_this_device);
EXPORT_SYMBOL_GPL(rs_file_operations_llseek);
EXPORT_SYMBOL_GPL(rs_file_operations_read_iter);
EXPORT_SYMBOL_GPL(rs_file_operations_write_iter);
EXPORT_SYMBOL_GPL(rs_file_operations_unlocked_ioctl);
#ifdef CONFIG_COMPAT
EXPORT_SYMBOL_GPL(rs_file_operations_compat_ioctl);
#endif
EXPORT_SYMBOL_GPL(rs_file_operations_mmap);
EXPORT_SYMBOL_GPL(rs_file_operations_open);
EXPORT_SYMBOL_GPL(rs_file_operations_release);
EXPORT_SYMBOL_GPL(rs_file_operations_owner);
EXPORT_SYMBOL_GPL(rs_file_operations_show_fdinfo);
EXPORT_SYMBOL_GPL(rs_file_operations_set_llseek);
EXPORT_SYMBOL_GPL(rs_file_operations_set_read_iter);
EXPORT_SYMBOL_GPL(rs_file_operations_set_write_iter);
EXPORT_SYMBOL_GPL(rs_file_operations_set_unlocked_ioctl);
#ifdef CONFIG_COMPAT
EXPORT_SYMBOL_GPL(rs_file_operations_set_compat_ioctl);
#endif
EXPORT_SYMBOL_GPL(rs_file_operations_set_mmap);
EXPORT_SYMBOL_GPL(rs_file_operations_set_open);
EXPORT_SYMBOL_GPL(rs_file_operations_set_release);
EXPORT_SYMBOL_GPL(rs_file_operations_set_owner);
EXPORT_SYMBOL_GPL(rs_file_operations_set_show_fdinfo);

__nocfi struct file *rs_kiocb_ki_filp(const struct kiocb *kiocb)
{
	return kiocb->ki_filp;
}

__nocfi loff_t rs_kiocb_ki_pos(const struct kiocb *kiocb)
{
	return kiocb->ki_pos;
}

__nocfi void rs_kiocb_set_ki_pos(struct kiocb *kiocb, loff_t value)
{
	kiocb->ki_pos = value;
}

EXPORT_SYMBOL_GPL(rs_kiocb_ki_filp);
EXPORT_SYMBOL_GPL(rs_kiocb_ki_pos);
EXPORT_SYMBOL_GPL(rs_kiocb_set_ki_pos);
