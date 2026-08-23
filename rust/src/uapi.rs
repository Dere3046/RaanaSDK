// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

//! Minimal UAPI ioctl constants used by `crate::ioctl`.
//!
//! C header: [`include/uapi/asm-generic/ioctl.h`](srctree/include/uapi/asm-generic/ioctl.h)

#![allow(non_upper_case_globals)]

pub const _IOC_NRBITS: u32 = 8;
pub const _IOC_TYPEBITS: u32 = 8;
pub const _IOC_SIZEBITS: u32 = 14;
pub const _IOC_DIRBITS: u32 = 2;

pub const _IOC_NRMASK: u32 = (1 << _IOC_NRBITS) - 1;
pub const _IOC_TYPEMASK: u32 = (1 << _IOC_TYPEBITS) - 1;
pub const _IOC_SIZEMASK: u32 = (1 << _IOC_SIZEBITS) - 1;
pub const _IOC_DIRMASK: u32 = (1 << _IOC_DIRBITS) - 1;

pub const _IOC_NRSHIFT: u32 = 0;
pub const _IOC_TYPESHIFT: u32 = _IOC_NRSHIFT + _IOC_NRBITS;
pub const _IOC_SIZESHIFT: u32 = _IOC_TYPESHIFT + _IOC_TYPEBITS;
pub const _IOC_DIRSHIFT: u32 = _IOC_SIZESHIFT + _IOC_SIZEBITS;

pub const _IOC_NONE: u32 = 0;
pub const _IOC_WRITE: u32 = 1;
pub const _IOC_READ: u32 = 2;
