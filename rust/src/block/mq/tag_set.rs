// SPDX-License-Identifier: GPL-2.0

//! This module provides the `TagSet` struct to wrap the C `struct blk_mq_tag_set`.
//!
//! C header: [`include/linux/blk-mq.h`](srctree/include/linux/blk-mq.h)

use core::pin::Pin;

use crate::{
    bindings,
    block::mq::{operations::OperationsVTable, request::RequestDataWrapper, Operations},
    error,
    init,
    prelude::PinInit,
};
use core::{convert::TryInto, marker::PhantomData};
use macros::{pin_data, pinned_drop};

extern "C" {
    #[link_name = "rs_blk_mq_tag_set_alloc"]
    fn rs_blk_mq_tag_set_alloc(
        ops: *const bindings::blk_mq_ops,
        nr_hw_queues: u32,
        num_tags: u32,
        num_maps: u32,
        cmd_size: u32,
        flags: u32,
        numa_node: crate::ffi::c_int,
        timeout: u32,
        driver_data: *mut crate::ffi::c_void,
    ) -> *mut bindings::blk_mq_tag_set;
    #[link_name = "rs_blk_mq_tag_set_free"]
    fn rs_blk_mq_tag_set_free(set: *mut bindings::blk_mq_tag_set);
}

/// A wrapper for the C `struct blk_mq_tag_set`.
///
/// `struct blk_mq_tag_set` contains a `struct list_head` and so must be pinned.
///
/// # Invariants
///
/// - `inner` is initialized and valid.
#[pin_data(PinnedDrop)]
#[repr(transparent)]
pub struct TagSet<T: Operations> {
    #[pin]
    inner: *mut bindings::blk_mq_tag_set,
    _p: PhantomData<T>,
}

impl<T: Operations> TagSet<T> {
    /// Try to create a new tag set
    pub fn new(
        nr_hw_queues: u32,
        num_tags: u32,
        num_maps: u32,
    ) -> impl PinInit<Self, error::Error> {
        // SAFETY: The closure writes every field of `TagSet` before returning `Ok`.
        unsafe {
            init::pin_init_from_closure(move |slot: *mut Self| {
                let inner = Self::alloc_tag_set(nr_hw_queues, num_tags, num_maps)?;
                core::ptr::write(&mut (*slot).inner, inner);
                Ok(())
            })
        }
    }

    fn alloc_tag_set(
        nr_hw_queues: u32,
        num_tags: u32,
        num_maps: u32,
    ) -> crate::error::Result<*mut bindings::blk_mq_tag_set> {
        let cmd_size = core::mem::size_of::<RequestDataWrapper>().try_into()?;
        let ops = OperationsVTable::<T>::build();
        if ops.is_null() {
            return Err(error::code::ENOMEM);
        }

        let set = crate::error::from_err_ptr(unsafe {
            rs_blk_mq_tag_set_alloc(
                ops,
                nr_hw_queues,
                num_tags,
                num_maps,
                cmd_size,
                bindings::BLK_MQ_F_SHOULD_MERGE,
                bindings::NUMA_NO_NODE,
                0,
                core::ptr::null_mut(),
            )
        })?;
        if set.is_null() {
            return Err(error::code::ENOMEM);
        }
        Ok(set)
    }

    /// Return the pointer to the wrapped `struct blk_mq_tag_set`
    pub(crate) fn raw_tag_set(&self) -> *mut bindings::blk_mq_tag_set {
        self.inner
    }
}

#[pinned_drop]
impl<T: Operations> PinnedDrop for TagSet<T> {
    fn drop(self: Pin<&mut Self>) {
        // SAFETY: By type invariant `inner` is valid and has been properly
        // initialized during construction. `rs_blk_mq_tag_set_free` releases both
        // the C tag set and the C blk-mq ops vtable owned by it.
        unsafe { rs_blk_mq_tag_set_free(self.inner) };
    }
}
