// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#![no_std]
#![feature(arbitrary_self_types)]
#![feature(asm_goto)]
#![feature(coerce_unsized)]
#![feature(dispatch_from_dyn)]
#![feature(unsize)]

extern crate self as kernel;

pub mod bindings;
pub mod ffi;
pub use macros;

pub mod alloc;
pub mod block;
pub mod build_assert;
pub mod cred;
pub mod device;
pub mod error;
pub mod firmware;
pub mod fs;
pub mod init;
pub mod ioctl;
pub mod uapi;
pub mod jump_label;
pub mod kunit;
pub mod list;
pub mod miscdevice;
pub mod mm;
pub mod net;
pub mod page;
pub mod page_size_compat;
pub mod pid_namespace;
pub mod prelude;
pub mod print;
pub mod rbtree;
pub mod security;
pub mod seq_file;
pub mod sizes;
pub mod static_assert;
#[doc(hidden)]
pub mod std_vendor;
pub mod str;
pub mod sync;
pub mod task;
pub mod time;
pub mod tracepoint;
pub mod types;
pub mod uaccess;
pub mod workqueue;

const __LOG_PREFIX: &[u8] = b"rust_support\0";

pub trait Module: Sized + Sync + Send {
    fn init(module: &'static ThisModule) -> error::Result<Self>;
}

pub struct ThisModule(*mut bindings::module);

unsafe impl Sync for ThisModule {}

impl ThisModule {
    pub const unsafe fn from_ptr(ptr: *mut bindings::module) -> ThisModule {
        ThisModule(ptr)
    }

    pub const fn as_ptr(&self) -> *mut bindings::module {
        self.0
    }
}

#[inline(never)]
#[cold]
#[export_name = "rust_build_error"]
#[track_caller]
pub const fn build_error(msg: &'static str) -> ! {
    panic!("{}", msg)
}

#[panic_handler]
fn panic(info: &core::panic::PanicInfo<'_>) -> ! {
    pr_emerg!("{}\n", info);
    unsafe { bindings::BUG() }
}

#[macro_export]
macro_rules! container_of {
    ($ptr:expr, $type:ty, $($f:tt)*) => {{
        let ptr = $ptr as *const _ as *const u8;
        let offset: usize = ::core::mem::offset_of!($type, $($f)*);
        ptr.sub(offset) as *const $type
    }}
}

#[doc(hidden)]
#[macro_export]
macro_rules! concat_literals {
    ($( $asm:literal )* ) => {
        ::core::concat!($($asm),*)
    };
}

#[macro_export]
macro_rules! asm {
    ($($asm:expr),* ; $($rest:tt)*) => {
        ::core::arch::asm!( $($asm)*, $($rest)* )
    };
}
