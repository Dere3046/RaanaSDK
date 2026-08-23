// SPDX-License-Identifier: GPL-2.0

//! Generic disk abstraction.
//!
//! C header: [`include/linux/blkdev.h`](srctree/include/linux/blkdev.h)
//! C header: [`include/linux/blk_mq.h`](srctree/include/linux/blk_mq.h)

use crate::block::mq::{raw_writer::RawWriter, Operations, TagSet};
use crate::{bindings, error::from_err_ptr, error::Result, sync::Arc};
use crate::{error, static_lock_class};
use core::fmt::{self, Write};

/// Size of `struct gendisk::disk_name` in the kernel (`DISK_NAME_LEN`).
const DISK_NAME_LEN: usize = 32;

extern "C" {
    #[link_name = "rs_blk_mq_alloc_disk_with_limits"]
    fn rs_blk_mq_alloc_disk_with_limits(
        tag_set: *mut bindings::blk_mq_tag_set,
        rotational: bool,
        logical_block_size: u32,
        physical_block_size: u32,
        queuedata: *mut crate::ffi::c_void,
        lkclass: *mut bindings::lock_class_key,
    ) -> *mut bindings::gendisk;
    #[link_name = "rs_block_device_operations_zero"]
    fn rs_block_device_operations_zero() -> *const bindings::block_device_operations;
    #[link_name = "rs_gendisk_set_fops"]
    fn rs_gendisk_set_fops(
        disk: *mut bindings::gendisk,
        fops: *const bindings::block_device_operations,
    );
    #[link_name = "rs_gendisk_disk_name"]
    fn rs_gendisk_disk_name(disk: *mut bindings::gendisk) -> *mut crate::ffi::c_char;
}

/// A builder for [`GenDisk`].
///
/// Use this struct to configure and add new [`GenDisk`] to the VFS.
pub struct GenDiskBuilder {
    rotational: bool,
    logical_block_size: u32,
    physical_block_size: u32,
    capacity_sectors: u64,
}

impl Default for GenDiskBuilder {
    fn default() -> Self {
        Self {
            rotational: false,
            logical_block_size: bindings::PAGE_SIZE as u32,
            physical_block_size: bindings::PAGE_SIZE as u32,
            capacity_sectors: 0,
        }
    }
}

impl GenDiskBuilder {
    /// Create a new instance.
    pub fn new() -> Self {
        Self::default()
    }

    /// Set the rotational media attribute for the device to be built.
    pub fn rotational(mut self, rotational: bool) -> Self {
        self.rotational = rotational;
        self
    }

    /// Validate block size by verifying that it is between 512 and `PAGE_SIZE`,
    /// and that it is a power of two.
    fn validate_block_size(size: u32) -> Result<()> {
        if !(512..=bindings::PAGE_SIZE as u32).contains(&size) || !size.is_power_of_two() {
            Err(error::code::EINVAL)
        } else {
            Ok(())
        }
    }

    /// Set the logical block size of the device to be built.
    ///
    /// This method will check that block size is a power of two and between 512
    /// and 4096. If not, an error is returned and the block size is not set.
    ///
    /// This is the smallest unit the storage device can address. It is
    /// typically 4096 bytes.
    pub fn logical_block_size(mut self, block_size: u32) -> Result<Self> {
        Self::validate_block_size(block_size)?;
        self.logical_block_size = block_size;
        Ok(self)
    }

    /// Set the physical block size of the device to be built.
    ///
    /// This method will check that block size is a power of two and between 512
    /// and 4096. If not, an error is returned and the block size is not set.
    ///
    /// This is the smallest unit a physical storage device can write
    /// atomically. It is usually the same as the logical block size but may be
    /// bigger. One example is SATA drives with 4096 byte physical block size
    /// that expose a 512 byte logical block size to the operating system.
    pub fn physical_block_size(mut self, block_size: u32) -> Result<Self> {
        Self::validate_block_size(block_size)?;
        self.physical_block_size = block_size;
        Ok(self)
    }

    /// Set the capacity of the device to be built, in sectors (512 bytes).
    pub fn capacity_sectors(mut self, capacity: u64) -> Self {
        self.capacity_sectors = capacity;
        self
    }

    /// Build a new `GenDisk` and add it to the VFS.
    pub fn build<T: Operations>(
        self,
        name: fmt::Arguments<'_>,
        tagset: Arc<TagSet<T>>,
    ) -> Result<GenDisk<T>> {
        // SAFETY: `tagset.raw_tag_set()` points to a valid and initialized tag set.
        let gendisk = from_err_ptr(unsafe {
            rs_blk_mq_alloc_disk_with_limits(
                tagset.raw_tag_set(),
                self.rotational,
                self.logical_block_size,
                self.physical_block_size,
                core::ptr::null_mut(),
                static_lock_class!().as_ptr(),
            )
        })?;
        if gendisk.is_null() {
            return Err(error::code::ENOMEM);
        }

        // SAFETY: `rs_block_device_operations_zero` returns a pointer to a static,
        // zero-initialized `struct block_device_operations` that is valid for the
        // lifetime of the module.
        let fops = unsafe { rs_block_device_operations_zero() };
        // SAFETY: `gendisk` is a valid pointer as we initialized it above.
        unsafe { rs_gendisk_set_fops(gendisk, fops) };

        let mut raw_writer = unsafe {
            RawWriter::from_raw_parts(
                // SAFETY: `gendisk` points to a valid and initialized instance. We
                // have exclusive access, since the disk is not added to the VFS
                // yet.
                rs_gendisk_disk_name(gendisk),
                DISK_NAME_LEN,
            )
        }?;
        raw_writer.write_fmt(name)?;
        raw_writer.write_char('\0')?;

        // SAFETY: `gendisk` points to a valid and initialized instance of
        // `struct gendisk`. `set_capacity` takes a lock to synchronize this
        // operation, so we will not race.
        unsafe { bindings::set_capacity(gendisk, self.capacity_sectors) };

        crate::error::to_result(
            // SAFETY: `gendisk` points to a valid and initialized instance of
            // `struct gendisk`.
            unsafe {
                bindings::device_add_disk(core::ptr::null_mut(), gendisk, core::ptr::null_mut())
            },
        )?;

        // INVARIANT: `gendisk` was initialized above.
        // INVARIANT: `gendisk` was added to the VFS via `device_add_disk` above.
        Ok(GenDisk {
            _tagset: tagset,
            gendisk,
        })
    }
}

/// A generic block device.
///
/// # Invariants
///
/// - `gendisk` must always point to an initialized and valid `struct gendisk`.
/// - `gendisk` was added to the VFS through a call to
///   `bindings::device_add_disk`.
pub struct GenDisk<T: Operations> {
    _tagset: Arc<TagSet<T>>,
    gendisk: *mut bindings::gendisk,
}

// SAFETY: `GenDisk` is an owned pointer to a `struct gendisk` and an `Arc` to a
// `TagSet` It is safe to send this to other threads as long as T is Send.
unsafe impl<T: Operations + Send> Send for GenDisk<T> {}

impl<T: Operations> Drop for GenDisk<T> {
    fn drop(&mut self) {
        // SAFETY: By type invariant, `self.gendisk` points to a valid and
        // initialized instance of `struct gendisk`, and it was previously added
        // to the VFS.
        unsafe { bindings::del_gendisk(self.gendisk) };
    }
}
