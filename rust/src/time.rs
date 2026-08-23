// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

//! Time related primitives.
//!
//! This module contains the kernel APIs related to time and timers that
//! have been ported or wrapped for usage by Rust code in the kernel.
//!
//! C header: [`include/linux/jiffies.h`](srctree/include/linux/jiffies.h).
//! C header: [`include/linux/ktime.h`](srctree/include/linux/ktime.h).

// Local FFI declarations for the time helpers that are not exposed through
// `bindings`. The `rs_` wrappers are runtime-resolved kernel functions provided
// by the C shim.
extern "C" {
    #[link_name = "rs___msecs_to_jiffies"]
    fn rs___msecs_to_jiffies(msecs: Msecs) -> Jiffies;
    #[link_name = "rs_ktime_get"]
    fn rs_ktime_get() -> i64;
}

/// The number of nanoseconds per millisecond.
pub const NSEC_PER_MSEC: i64 = 1_000_000;

/// The time unit of Linux kernel. One jiffy equals (1/HZ) second.
pub type Jiffies = crate::ffi::c_ulong;

/// The millisecond time unit.
pub type Msecs = crate::ffi::c_uint;

/// Converts milliseconds to jiffies.
#[inline]
pub fn msecs_to_jiffies(msecs: Msecs) -> Jiffies {
    // SAFETY: The `__msecs_to_jiffies` function is always safe to call no
    // matter what the argument is.
    unsafe { rs___msecs_to_jiffies(msecs) }
}

/// A Rust wrapper around a `ktime_t`.
#[repr(transparent)]
#[derive(Copy, Clone)]
pub struct Ktime {
    inner: i64,
}

impl Ktime {
    /// Create a `Ktime` from a raw `ktime_t`.
    #[inline]
    pub fn from_raw(inner: i64) -> Self {
        Self { inner }
    }

    /// Get the current time using `CLOCK_MONOTONIC`.
    #[inline]
    pub fn ktime_get() -> Self {
        // SAFETY: It is always safe to call `ktime_get` outside of NMI context.
        Self::from_raw(unsafe { rs_ktime_get() })
    }

    /// Divide the number of nanoseconds by a compile-time constant.
    #[inline]
    fn divns_constant<const DIV: i64>(self) -> i64 {
        self.to_ns() / DIV
    }

    /// Returns the number of nanoseconds.
    #[inline]
    pub fn to_ns(self) -> i64 {
        self.inner
    }

    /// Returns the number of milliseconds.
    #[inline]
    pub fn to_ms(self) -> i64 {
        self.divns_constant::<NSEC_PER_MSEC>()
    }
}

/// Returns the number of milliseconds between two ktimes.
#[inline]
pub fn ktime_ms_delta(later: Ktime, earlier: Ktime) -> i64 {
    (later - earlier).to_ms()
}

impl core::ops::Sub for Ktime {
    type Output = Ktime;

    #[inline]
    fn sub(self, other: Ktime) -> Ktime {
        Self {
            inner: self.inner - other.inner,
        }
    }
}
