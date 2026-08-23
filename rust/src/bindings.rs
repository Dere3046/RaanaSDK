// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#![allow(non_camel_case_types, non_snake_case, non_upper_case_globals, missing_docs, improper_ctypes, clippy::all)]

use core::ffi::{c_char, c_int, c_long, c_void};

pub type c_uint = u32;
pub type c_ulong = usize;
pub type gfp_t = u32;
pub type pid_t = i32;
pub type uid_t = u32;
pub type loff_t = i64;
pub type vm_flags_t = c_ulong;
pub type blk_features_t = u32;
pub type blk_status_t = u8;
pub type kunit_speed = u32;
pub type kunit_status = u32;

#[derive(Copy, Clone)]
#[repr(C)]
pub struct kuid_t {
    pub val: uid_t,
}

#[repr(C)]
pub struct atomic_t {
    pub counter: c_int,
}

#[repr(C)]
pub struct refcount_t {
    pub refs: atomic_t,
}

#[repr(C)]
pub struct sched_param {
    pub sched_priority: c_int,
}

#[repr(C)]
pub struct task_struct {
    _private: [u8; 1],
}

#[repr(C)]
pub struct user_namespace {
    _private: [u8; 1],
}

#[repr(C)]
pub struct pid_namespace {
    _private: [u8; 1],
}

#[repr(C)]
pub struct page {
    _private: [u8; 1],
}

#[repr(C)]
pub struct mm_struct {
    _private: [u8; 1],
}

#[repr(C)]
pub struct vm_area_struct {
    _private: [u8; 1],
}

#[repr(C)]
pub struct cred {
    _private: [u8; 1],
}

#[repr(C)]
pub struct file {
    _private: [u8; 1],
}

#[repr(C)]
pub struct inode {
    _private: [u8; 1],
}

#[repr(C)]
pub struct seq_file {
    _private: [u8; 1],
}

#[repr(C)]
pub struct wait_queue_head {
    _private: [u8; 1],
}

#[repr(C)]
pub struct wait_queue_entry {
    _private: [u8; 1],
}

#[repr(C)]
pub struct poll_table_struct {
    _private: [u8; 1],
}

pub type poll_table = poll_table_struct;

#[repr(C)]
pub struct mutex {
    _private: [u8; 1],
}

#[repr(C)]
pub struct spinlock_t {
    _private: [u8; 1],
}

#[repr(C)]
pub struct lock_class_key {
    _private: [u8; 1],
}

#[repr(C)]
pub struct module {
    _private: [u8; 1],
}

#[repr(C)]
pub struct device {
    _private: [u8; 1],
}

#[repr(C)]
pub struct firmware {
    _private: [u8; 1],
}

#[repr(C)]
pub struct kiocb {
    _private: [u8; 1],
}

#[repr(C)]
pub struct iov_iter {
    _private: [u8; 1],
}

#[repr(C)]
pub struct file_operations {
    _private: [u8; 1],
}

#[repr(C)]
pub struct miscdevice {
    _private: [u8; 1],
}

#[repr(C)]
pub struct workqueue_struct {
    _private: [u8; 1],
}

#[repr(C)]
pub struct work_struct {
    _private: [u8; 1],
}

#[repr(C)]
pub struct queue_limits {
    _private: [u8; 1],
}

#[repr(C)]
pub struct block_device_operations {
    _private: [u8; 1],
}

#[repr(C)]
pub struct gendisk {
    _private: [u8; 1],
}

#[repr(C)]
pub struct blk_mq_hw_ctx {
    _private: [u8; 1],
}

#[repr(C)]
pub struct blk_mq_queue_data {
    _private: [u8; 1],
}

#[repr(C)]
pub struct blk_mq_tag_set {
    _private: [u8; 1],
}

#[repr(C)]
pub struct blk_mq_ops {
    _private: [u8; 1],
}

#[repr(C)]
pub struct request {
    _private: [u8; 1],
}

#[repr(C)]
pub struct io_comp_batch {
    _private: [u8; 1],
}

#[derive(Copy, Clone, Default)]
#[repr(C)]
pub struct rb_node {
    _private: [u8; 1],
}

#[derive(Copy, Clone, Default)]
#[repr(C)]
pub struct rb_root {
    _private: [u8; 1],
}

#[derive(Copy, Clone)]
#[repr(C)]
pub struct static_key {
    _private: [u8; 1],
}

#[derive(Copy, Clone)]
#[repr(C)]
pub struct static_key_false {
    pub key: static_key,
}

#[repr(C)]
pub struct kunit {
    _private: [u8; 1],
}

#[derive(Copy, Clone)]
#[repr(C)]
pub struct kunit_attributes {
    pub speed: kunit_speed,
    pub __kabi_reserved1: c_ulong,
}

#[derive(Copy, Clone)]
#[repr(C)]
pub struct kunit_case {
    _private: [u8; 1],
}

#[repr(C)]
pub struct kunit_suite {
    _private: [u8; 1],
}

pub const KERN_EMERG: [u8; 3] = [0x01, b'0', 0];
pub const KERN_ALERT: [u8; 3] = [0x01, b'1', 0];
pub const KERN_CRIT: [u8; 3] = [0x01, b'2', 0];
pub const KERN_ERR: [u8; 3] = [0x01, b'3', 0];
pub const KERN_WARNING: [u8; 3] = [0x01, b'4', 0];
pub const KERN_NOTICE: [u8; 3] = [0x01, b'5', 0];
pub const KERN_INFO: [u8; 3] = [0x01, b'6', 0];
pub const KERN_DEBUG: [u8; 3] = [0x01, b'7', 0];
pub const KERN_CONT: [u8; 3] = [0x01, b'c', 0];

pub const TASK_INTERRUPTIBLE: c_int = 0x00000001;
pub const TASK_UNINTERRUPTIBLE: c_int = 0x00000002;
pub const TASK_FREEZABLE: c_int = 0x00002000;
pub const TASK_NORMAL: c_uint = 3;
pub const MAX_NICE: c_int = 19;
pub const SCHED_RESET_ON_FORK: c_int = 0x40000000;
pub const RLIMIT_NICE: c_uint = 13;
pub const RLIMIT_RTPRIO: c_uint = 14;
pub const PAGE_SIZE: usize = 4096;
pub const PAGE_SHIFT: usize = 12;
pub const MAX_ERRNO: c_int = 4095;

pub const __GFP_HIGHMEM: gfp_t = 0x2;
pub const __GFP_HIGH: gfp_t = 0x20;
pub const __GFP_IO: gfp_t = 0x40;
pub const __GFP_FS: gfp_t = 0x80;
pub const __GFP_ZERO: gfp_t = 0x100;
pub const __GFP_DIRECT_RECLAIM: gfp_t = 0x400;
pub const __GFP_KSWAPD_RECLAIM: gfp_t = 0x800;
pub const __GFP_NOWARN: gfp_t = 0x2000;
pub const __GFP_ACCOUNT: gfp_t = 0x400000;
pub const GFP_ATOMIC: gfp_t = __GFP_HIGH | __GFP_KSWAPD_RECLAIM;
pub const GFP_KERNEL: gfp_t = __GFP_DIRECT_RECLAIM | __GFP_KSWAPD_RECLAIM | __GFP_IO | __GFP_FS;
pub const GFP_KERNEL_ACCOUNT: gfp_t = GFP_KERNEL | __GFP_ACCOUNT;
pub const GFP_NOWAIT: gfp_t = __GFP_KSWAPD_RECLAIM | __GFP_NOWARN;

pub const SZ_1K: usize = 0x400;
pub const SZ_2K: usize = 0x800;
pub const SZ_4K: usize = 0x1000;
pub const SZ_8K: usize = 0x2000;
pub const SZ_16K: usize = 0x4000;
pub const SZ_32K: usize = 0x8000;
pub const SZ_64K: usize = 0x10000;
pub const SZ_128K: usize = 0x20000;
pub const SZ_256K: usize = 0x40000;
pub const SZ_512K: usize = 0x80000;

pub const VM_NONE: vm_flags_t = 0x00000000;
pub const VM_READ: vm_flags_t = 0x00000001;
pub const VM_WRITE: vm_flags_t = 0x00000002;
pub const VM_EXEC: vm_flags_t = 0x00000004;
pub const VM_SHARED: vm_flags_t = 0x00000008;
pub const VM_MAYREAD: vm_flags_t = 0x00000010;
pub const VM_MAYWRITE: vm_flags_t = 0x00000020;
pub const VM_MAYEXEC: vm_flags_t = 0x00000040;
pub const VM_MAYSHARE: vm_flags_t = 0x00000080;
pub const VM_PFNMAP: vm_flags_t = 0x00000400;
pub const VM_IO: vm_flags_t = 0x00004000;
pub const VM_DONTCOPY: vm_flags_t = 0x00020000;
pub const VM_DONTEXPAND: vm_flags_t = 0x00040000;
pub const VM_LOCKONFAULT: vm_flags_t = 0x00080000;
pub const VM_ACCOUNT: vm_flags_t = 0x00100000;
pub const VM_NORESERVE: vm_flags_t = 0x00200000;
pub const VM_HUGETLB: vm_flags_t = 0x00400000;
pub const VM_SYNC: vm_flags_t = 0x00800000;
pub const VM_ARCH_1: vm_flags_t = 0x01000000;
pub const VM_WIPEONFORK: vm_flags_t = 0x02000000;
pub const VM_DONTDUMP: vm_flags_t = 0x04000000;
pub const VM_SOFTDIRTY: vm_flags_t = 0x08000000;
pub const VM_MIXEDMAP: vm_flags_t = 0x10000000;
pub const VM_HUGEPAGE: vm_flags_t = 0x20000000;
pub const VM_NOHUGEPAGE: vm_flags_t = 0x40000000;
pub const VM_MERGEABLE: vm_flags_t = 0x80000000;

pub const BLK_FEAT_ROTATIONAL: blk_features_t = 1 << 2;
pub const BLK_STS_OK: blk_status_t = 0;
pub const NUMA_NO_NODE: c_int = -1;
pub const BLK_MQ_F_SHOULD_MERGE: u32 = 1;
pub const MISC_DYNAMIC_MINOR: c_int = 255;
pub const wq_misc_consts_WORK_CPU_UNBOUND: c_int = 256;
pub const kunit_speed_KUNIT_SPEED_NORMAL: kunit_speed = 0;
pub const kunit_status_KUNIT_SUCCESS: kunit_status = 0;

pub const EPERM: c_int = 1;
pub const ENOENT: c_int = 2;
pub const ESRCH: c_int = 3;
pub const EINTR: c_int = 4;
pub const EIO: c_int = 5;
pub const ENXIO: c_int = 6;
pub const E2BIG: c_int = 7;
pub const ENOEXEC: c_int = 8;
pub const EBADF: c_int = 9;
pub const ECHILD: c_int = 10;
pub const EAGAIN: c_int = 11;
pub const ENOMEM: c_int = 12;
pub const EACCES: c_int = 13;
pub const EFAULT: c_int = 14;
pub const ENOTBLK: c_int = 15;
pub const EBUSY: c_int = 16;
pub const EEXIST: c_int = 17;
pub const EXDEV: c_int = 18;
pub const ENODEV: c_int = 19;
pub const ENOTDIR: c_int = 20;
pub const EISDIR: c_int = 21;
pub const EINVAL: c_int = 22;
pub const ENFILE: c_int = 23;
pub const EMFILE: c_int = 24;
pub const ENOTTY: c_int = 25;
pub const ETXTBSY: c_int = 26;
pub const EFBIG: c_int = 27;
pub const ENOSPC: c_int = 28;
pub const ESPIPE: c_int = 29;
pub const EROFS: c_int = 30;
pub const EMLINK: c_int = 31;
pub const EPIPE: c_int = 32;
pub const EDOM: c_int = 33;
pub const ERANGE: c_int = 34;
pub const ERESTARTSYS: c_int = 512;
pub const ERESTARTNOINTR: c_int = 513;
pub const ERESTARTNOHAND: c_int = 514;
pub const ENOIOCTLCMD: c_int = 515;
pub const ERESTART_RESTARTBLOCK: c_int = 516;
pub const EPROBE_DEFER: c_int = 517;
pub const EOPENSTALE: c_int = 518;
pub const ENOPARAM: c_int = 519;
pub const EBADHANDLE: c_int = 521;
pub const ENOTSYNC: c_int = 522;
pub const EBADCOOKIE: c_int = 523;
pub const ENOTSUPP: c_int = 524;
pub const ETOOSMALL: c_int = 525;
pub const ESERVERFAULT: c_int = 526;
pub const EBADTYPE: c_int = 527;
pub const EJUKEBOX: c_int = 528;
pub const EIOCBQUEUED: c_int = 529;
pub const ERECALLCONFLICT: c_int = 530;
pub const ENOGRACE: c_int = 531;

extern "C" {
    #[link_name = "rust_helper_BUG"]
    pub fn BUG() -> !;

    #[link_name = "rust_helper_ERR_PTR"]
    pub fn ERR_PTR(err: c_long) -> *mut c_void;
    #[link_name = "rust_helper_IS_ERR"]
    pub fn IS_ERR(ptr: *const c_void) -> bool;
    #[link_name = "rust_helper_PTR_ERR"]
    pub fn PTR_ERR(ptr: *const c_void) -> c_long;
    #[link_name = "rust_helper_errname"]
    pub fn errname(err: c_int) -> *const c_char;

    #[link_name = "rust_helper_get_current"]
    pub fn get_current() -> *mut task_struct;
    #[link_name = "rust_helper_get_task_struct"]
    pub fn get_task_struct(t: *mut task_struct);
    #[link_name = "rust_helper_put_task_struct"]
    pub fn put_task_struct(t: *mut task_struct);
    #[link_name = "rust_helper_task_uid"]
    pub fn task_uid(task: *mut task_struct) -> kuid_t;
    #[link_name = "rust_helper_task_euid"]
    pub fn task_euid(task: *mut task_struct) -> kuid_t;
    #[link_name = "rust_helper_from_kuid"]
    pub fn from_kuid(to: *mut user_namespace, uid: kuid_t) -> uid_t;
    #[link_name = "rust_helper_uid_eq"]
    pub fn uid_eq(left: kuid_t, right: kuid_t) -> bool;
    #[link_name = "rust_helper_current_euid"]
    pub fn current_euid() -> kuid_t;
    #[link_name = "rust_helper_current_user_ns"]
    pub fn current_user_ns() -> *mut user_namespace;
    #[link_name = "rust_helper_task_tgid_nr_ns"]
    pub fn task_tgid_nr_ns(tsk: *mut task_struct, ns: *mut pid_namespace) -> pid_t;
    #[link_name = "rust_helper_task_rlimit"]
    pub fn task_rlimit(task: *const task_struct, limit: c_uint) -> c_ulong;
    #[link_name = "rust_helper_get_pid_ns"]
    pub fn get_pid_ns(ns: *mut pid_namespace) -> *mut pid_namespace;
    #[link_name = "rust_helper_put_pid_ns"]
    pub fn put_pid_ns(ns: *mut pid_namespace);
    #[link_name = "rust_helper_task_get_pid_ns"]
    pub fn task_get_pid_ns(task: *mut task_struct) -> *mut pid_namespace;

    #[link_name = "rust_helper_alloc_pages"]
    pub fn alloc_pages(gfp_mask: gfp_t, order: c_uint) -> *mut page;
    #[link_name = "rust_helper_kmap_local_page"]
    pub fn kmap_local_page(page: *mut page) -> *mut c_void;
    #[link_name = "rust_helper_kunmap_local"]
    pub fn kunmap_local(addr: *const c_void);

    #[link_name = "rust_helper_copy_from_user"]
    pub fn copy_from_user(to: *mut c_void, from: *const c_void, n: c_ulong) -> c_ulong;
    #[link_name = "rust_helper_copy_to_user"]
    pub fn copy_to_user(to: *mut c_void, from: *const c_void, n: c_ulong) -> c_ulong;

    #[link_name = "rust_helper_krealloc"]
    pub fn krealloc(objp: *const c_void, new_size: usize, flags: gfp_t) -> *mut c_void;
    #[link_name = "rust_helper_kvrealloc"]
    pub fn kvrealloc(p: *const c_void, size: usize, flags: gfp_t) -> *mut c_void;
    #[link_name = "rust_helper_vrealloc"]
    pub fn vrealloc(p: *const c_void, size: usize, flags: gfp_t) -> *mut c_void;

    #[link_name = "rust_helper_signal_pending"]
    pub fn signal_pending(t: *mut task_struct) -> c_int;
    #[link_name = "rust_helper_poll_wait"]
    pub fn poll_wait(filp: *mut file, wait_address: *mut wait_queue_head, p: *mut poll_table_struct);
    #[link_name = "rust_helper_init_wait"]
    pub fn init_wait(wq_entry: *mut wait_queue_entry);

    #[link_name = "rust_helper___mutex_init"]
    pub fn __mutex_init(mutex: *mut mutex, name: *const c_char, key: *mut lock_class_key);
    #[link_name = "rust_helper_mutex_lock"]
    pub fn mutex_lock(lock: *mut mutex);
    #[link_name = "rust_helper___spin_lock_init"]
    pub fn __spin_lock_init(lock: *mut spinlock_t, name: *const c_char, key: *mut lock_class_key);
    #[link_name = "rust_helper_spin_lock"]
    pub fn spin_lock(lock: *mut spinlock_t);
    #[link_name = "rust_helper_spin_unlock"]
    pub fn spin_unlock(lock: *mut spinlock_t);
    #[link_name = "rust_helper_spin_trylock"]
    pub fn spin_trylock(lock: *mut spinlock_t) -> c_int;

    #[link_name = "rust_helper_REFCOUNT_INIT"]
    pub fn REFCOUNT_INIT(n: c_int) -> refcount_t;
    #[link_name = "rust_helper_refcount_inc"]
    pub fn refcount_inc(r: *mut refcount_t);
    #[link_name = "rust_helper_refcount_dec_and_test"]
    pub fn refcount_dec_and_test(r: *mut refcount_t) -> bool;

    #[link_name = "rust_helper_get_cred"]
    pub fn get_cred(cred: *const cred) -> *const cred;
    #[link_name = "rust_helper_put_cred"]
    pub fn put_cred(cred: *const cred);
    #[link_name = "rust_helper_get_file"]
    pub fn get_file(f: *mut file) -> *mut file;
    #[link_name = "rust_helper_i_size_read"]
    pub fn i_size_read(inode: *const inode) -> loff_t;

    #[link_name = "rs__printk"]
    pub fn _printk(fmt: *const c_char, args: *const c_void, ...) -> c_int;
    #[link_name = "rs_seq_printf"]
    pub fn seq_printf(m: *mut seq_file, fmt: *const c_char, args: *const c_void) -> c_int;
    #[link_name = "rs___free_pages"]
    pub fn __free_pages(page: *mut page, order: c_uint);
    #[link_name = "rs__copy_from_user"]
    pub fn _copy_from_user(to: *mut c_void, from: *const c_void, n: c_ulong) -> c_ulong;
    #[link_name = "rs__copy_to_user"]
    pub fn _copy_to_user(to: *mut c_void, from: *const c_void, n: c_ulong) -> c_ulong;
    #[link_name = "rs_strncpy_from_user"]
    pub fn strncpy_from_user(dst: *mut c_char, src: *const c_void, count: c_long) -> c_long;
    #[link_name = "rs_memchr"]
    pub fn memchr(s: *const c_void, c: c_int, n: usize) -> *mut c_void;
    #[link_name = "rs_strlen"]
    pub fn strlen(s: *const c_char) -> usize;
    #[link_name = "rs___init_waitqueue_head"]
    pub fn __init_waitqueue_head(wq_head: *mut wait_queue_head, name: *const c_char, key: *mut lock_class_key);
    #[link_name = "rs_prepare_to_wait_exclusive"]
    pub fn prepare_to_wait_exclusive(wq_head: *mut wait_queue_head, wq_entry: *mut wait_queue_entry, state: c_int);
    #[link_name = "rs_schedule_timeout"]
    pub fn schedule_timeout(timeout: c_long) -> c_long;
    #[link_name = "rs___wake_up"]
    pub fn __wake_up(q: *mut wait_queue_head, mode: c_uint, nr_exclusive: c_int, key: *mut c_void);
    #[link_name = "rs___wake_up_sync"]
    pub fn __wake_up_sync(q: *mut wait_queue_head, mode: c_uint);
    #[link_name = "rs_finish_wait"]
    pub fn finish_wait(wq_head: *mut wait_queue_head, wq_entry: *mut wait_queue_entry);
    #[link_name = "rs_synchronize_rcu"]
    pub fn synchronize_rcu();
    #[link_name = "rs___wake_up_pollfree"]
    pub fn __wake_up_pollfree(wq_head: *mut wait_queue_head);
    #[link_name = "rs_set_user_nice"]
    pub fn set_user_nice(p: *mut task_struct, nice: c_long);
    #[link_name = "rs_sched_setscheduler_nocheck"]
    pub fn sched_setscheduler_nocheck(p: *mut task_struct, policy: c_int, param: *const sched_param) -> c_int;
    #[link_name = "rs_has_capability_noaudit"]
    pub fn has_capability_noaudit(t: *mut task_struct, cap: c_int) -> bool;
    #[link_name = "rs_task_active_pid_ns"]
    pub fn task_active_pid_ns(tsk: *mut task_struct) -> *mut pid_namespace;
    #[link_name = "rs_wake_up_process"]
    pub fn wake_up_process(tsk: *mut task_struct) -> c_int;
    #[link_name = "rs_mutex_unlock"]
    pub fn mutex_unlock(lock: *mut mutex);
    #[link_name = "rs_mutex_trylock"]
    pub fn mutex_trylock(lock: *mut mutex) -> c_int;

    #[link_name = "rs_task_pid"]
    pub fn rs_task_pid(t: *mut task_struct) -> pid_t;
    #[link_name = "rs_task_tgid"]
    pub fn rs_task_tgid(t: *mut task_struct) -> pid_t;
    #[link_name = "rs_task_group_leader"]
    pub fn rs_task_group_leader(t: *mut task_struct) -> *mut task_struct;
    #[link_name = "rs_task_mm"]
    pub fn rs_task_mm(t: *mut task_struct) -> *mut mm_struct;
    #[link_name = "rs_task_policy"]
    pub fn rs_task_policy(t: *mut task_struct) -> c_uint;
    #[link_name = "rs_task_normal_prio"]
    pub fn rs_task_normal_prio(t: *mut task_struct) -> c_int;
    #[link_name = "rs_task_cred"]
    pub fn rs_task_cred(t: *mut task_struct) -> *const cred;
    #[link_name = "rs_page_nid"]
    pub fn rs_page_nid(p: *mut page) -> c_int;

    #[link_name = "rust_helper_blk_mq_rq_to_pdu"]
    pub fn blk_mq_rq_to_pdu(rq: *mut request) -> *mut c_void;
    #[link_name = "rust_helper_init_work_with_key"]
    pub fn init_work_with_key(
        work: *mut work_struct,
        func: Option<unsafe extern "C" fn(*mut work_struct)>,
        onstack: bool,
        name: *const c_char,
        key: *mut lock_class_key,
    );
    #[link_name = "rust_helper_kunit_get_current_test"]
    pub fn kunit_get_current_test() -> *mut kunit;
    #[link_name = "rust_helper_mmgrab"]
    pub fn mmgrab(mm: *mut mm_struct);
    #[link_name = "rust_helper_mmdrop"]
    pub fn mmdrop(mm: *mut mm_struct);
    #[link_name = "rust_helper_mmget"]
    pub fn mmget(mm: *mut mm_struct);
    #[link_name = "rust_helper_mmget_not_zero"]
    pub fn mmget_not_zero(mm: *mut mm_struct) -> bool;
    #[link_name = "rust_helper_mmap_read_lock"]
    pub fn mmap_read_lock(mm: *mut mm_struct);
    #[link_name = "rust_helper_mmap_read_trylock"]
    pub fn mmap_read_trylock(mm: *mut mm_struct) -> bool;
    #[link_name = "rust_helper_mmap_read_unlock"]
    pub fn mmap_read_unlock(mm: *mut mm_struct);
    #[link_name = "rust_helper_vma_lookup"]
    pub fn vma_lookup(mm: *mut mm_struct, addr: c_ulong) -> *mut vm_area_struct;
    #[link_name = "rust_helper_vma_end_read"]
    pub fn vma_end_read(vma: *mut vm_area_struct);
    #[link_name = "rust_helper_rb_link_node"]
    pub fn rb_link_node(node: *mut rb_node, parent: *mut rb_node, rb_link: *mut *mut rb_node);
    #[link_name = "rust_helper_security_cred_getsecid"]
    pub fn security_cred_getsecid(cred: *const cred, secid: *mut u32);
    #[link_name = "rust_helper_security_secid_to_secctx"]
    pub fn security_secid_to_secctx(secid: u32, secdata: *mut *mut c_char, seclen: *mut u32) -> c_int;
    #[link_name = "rust_helper_security_release_secctx"]
    pub fn security_release_secctx(secdata: *mut c_char, seclen: u32);
    #[link_name = "rust_helper_security_binder_set_context_mgr"]
    pub fn security_binder_set_context_mgr(mgr: *const cred) -> c_int;
    #[link_name = "rust_helper_security_binder_transaction"]
    pub fn security_binder_transaction(from: *const cred, to: *const cred) -> c_int;
    #[link_name = "rust_helper_security_binder_transfer_binder"]
    pub fn security_binder_transfer_binder(from: *const cred, to: *const cred) -> c_int;
    #[link_name = "rust_helper_security_binder_transfer_file"]
    pub fn security_binder_transfer_file(from: *const cred, to: *const cred, file: *mut file) -> c_int;
    #[link_name = "rust_helper_static_key_count"]
    pub fn static_key_count(key: *mut static_key) -> c_int;

    #[link_name = "rs_mmput"]
    pub fn mmput(mm: *mut mm_struct);
    #[link_name = "rs_mmput_async"]
    pub fn mmput_async(mm: *mut mm_struct);
    #[link_name = "rs_zap_page_range_single"]
    pub fn zap_page_range_single(
        vma: *mut vm_area_struct,
        address: c_ulong,
        size: c_ulong,
        details: *mut c_void,
    );
    #[link_name = "rs_vm_insert_page"]
    pub fn vm_insert_page(vma: *mut vm_area_struct, addr: c_ulong, page: *mut page) -> c_int;
    #[link_name = "rs_get_device"]
    pub fn get_device(dev: *mut device) -> *mut device;
    #[link_name = "rs_put_device"]
    pub fn put_device(dev: *mut device);
    #[link_name = "rs_request_firmware"]
    pub fn request_firmware(fw: *mut *const firmware, name: *const c_char, dev: *mut device) -> c_int;
    #[link_name = "rs_firmware_request_nowarn"]
    pub fn firmware_request_nowarn(fw: *mut *const firmware, name: *const c_char, dev: *mut device) -> c_int;
    #[link_name = "rs_release_firmware"]
    pub fn release_firmware(fw: *mut firmware);
    #[link_name = "rs_misc_register"]
    pub fn misc_register(misc: *mut miscdevice) -> c_int;
    #[link_name = "rs_misc_deregister"]
    pub fn misc_deregister(misc: *mut miscdevice) -> c_int;
    #[link_name = "rs__copy_from_iter"]
    pub fn _copy_from_iter(to: *mut c_void, len: usize, i: *mut iov_iter) -> usize;
    #[link_name = "rs__copy_to_iter"]
    pub fn _copy_to_iter(from: *const c_void, len: usize, i: *mut iov_iter) -> usize;
    #[link_name = "rs_generic_file_open"]
    pub fn generic_file_open(inode: *mut inode, file: *mut file) -> c_int;
    #[link_name = "rs___blk_mq_alloc_disk"]
    pub fn __blk_mq_alloc_disk(
        set: *mut blk_mq_tag_set,
        lim: *mut queue_limits,
        queuedata: *mut c_void,
        lkclass: *mut lock_class_key,
    ) -> *mut gendisk;
    #[link_name = "rs_set_capacity"]
    pub fn set_capacity(disk: *mut gendisk, sectors: u64);
    #[link_name = "rs_device_add_disk"]
    pub fn device_add_disk(
        parent: *mut device,
        disk: *mut gendisk,
        groups: *const *const c_char,
    ) -> c_int;
    #[link_name = "rs_del_gendisk"]
    pub fn del_gendisk(disk: *mut gendisk);
    #[link_name = "rs_blk_mq_start_request"]
    pub fn blk_mq_start_request(rq: *mut request);
    #[link_name = "rs_blk_mq_end_request"]
    pub fn blk_mq_end_request(rq: *mut request, error: blk_status_t);
    #[link_name = "rs_blk_mq_alloc_tag_set"]
    pub fn blk_mq_alloc_tag_set(set: *mut blk_mq_tag_set) -> c_int;
    #[link_name = "rs_blk_mq_free_tag_set"]
    pub fn blk_mq_free_tag_set(set: *mut blk_mq_tag_set);
    #[link_name = "rs_errno_to_blk_status"]
    pub fn errno_to_blk_status(err: c_int) -> blk_status_t;
    #[link_name = "rs_queue_work_on"]
    pub fn queue_work_on(cpu: c_int, wq: *mut workqueue_struct, work: *mut work_struct) -> bool;
    #[link_name = "rs_rb_first"]
    pub fn rb_first(root: *const rb_root) -> *mut rb_node;
    #[link_name = "rs_rb_last"]
    pub fn rb_last(root: *const rb_root) -> *mut rb_node;
    #[link_name = "rs_rb_first_postorder"]
    pub fn rb_first_postorder(root: *const rb_root) -> *mut rb_node;
    #[link_name = "rs_rb_next_postorder"]
    pub fn rb_next_postorder(node: *mut rb_node) -> *mut rb_node;
    #[link_name = "rs_rb_prev"]
    pub fn rb_prev(node: *mut rb_node) -> *mut rb_node;
    #[link_name = "rs_rb_next"]
    pub fn rb_next(node: *mut rb_node) -> *mut rb_node;
    #[link_name = "rs_rb_erase"]
    pub fn rb_erase(node: *mut rb_node, root: *mut rb_root);
    #[link_name = "rs_rb_insert_color"]
    pub fn rb_insert_color(node: *mut rb_node, root: *mut rb_root);
    #[link_name = "rs_rb_replace_node"]
    pub fn rb_replace_node(old: *mut rb_node, new: *mut rb_node, root: *mut rb_root);

    // Firmware accessors.
    #[link_name = "rs_firmware_size"]
    pub fn rs_firmware_size(obj: *const firmware) -> usize;
    #[link_name = "rs_firmware_data"]
    pub fn rs_firmware_data(obj: *const firmware) -> *const u8;
    // kiocb accessors.
    #[link_name = "rs_kiocb_ki_filp"]
    pub fn rs_kiocb_ki_filp(obj: *const kiocb) -> *mut file;
    #[link_name = "rs_kiocb_ki_pos"]
    pub fn rs_kiocb_ki_pos(obj: *const kiocb) -> loff_t;
    #[link_name = "rs_kiocb_set_ki_pos"]
    pub fn rs_kiocb_set_ki_pos(obj: *mut kiocb, value: loff_t);
    // miscdevice accessors.
    #[link_name = "rs_miscdevice_minor"]
    pub fn rs_miscdevice_minor(obj: *const miscdevice) -> c_int;
    #[link_name = "rs_miscdevice_set_minor"]
    pub fn rs_miscdevice_set_minor(obj: *mut miscdevice, value: c_int);
    #[link_name = "rs_miscdevice_name"]
    pub fn rs_miscdevice_name(obj: *const miscdevice) -> *const c_char;
    #[link_name = "rs_miscdevice_set_name"]
    pub fn rs_miscdevice_set_name(obj: *mut miscdevice, value: *const c_char);
    #[link_name = "rs_miscdevice_fops"]
    pub fn rs_miscdevice_fops(obj: *const miscdevice) -> *const file_operations;
    #[link_name = "rs_miscdevice_set_fops"]
    pub fn rs_miscdevice_set_fops(obj: *mut miscdevice, value: *const file_operations);
    #[link_name = "rs_miscdevice_this_device"]
    pub fn rs_miscdevice_this_device(obj: *const miscdevice) -> *mut device;
    #[link_name = "rs_miscdevice_set_this_device"]
    pub fn rs_miscdevice_set_this_device(obj: *mut miscdevice, value: *mut device);
    // queue_limits accessors.
    #[link_name = "rs_queue_limits_features"]
    pub fn rs_queue_limits_features(obj: *const queue_limits) -> blk_features_t;
    #[link_name = "rs_queue_limits_set_features"]
    pub fn rs_queue_limits_set_features(obj: *mut queue_limits, value: blk_features_t);
    #[link_name = "rs_queue_limits_logical_block_size"]
    pub fn rs_queue_limits_logical_block_size(obj: *const queue_limits) -> u32;
    #[link_name = "rs_queue_limits_set_logical_block_size"]
    pub fn rs_queue_limits_set_logical_block_size(obj: *mut queue_limits, value: u32);
    #[link_name = "rs_queue_limits_physical_block_size"]
    pub fn rs_queue_limits_physical_block_size(obj: *const queue_limits) -> u32;
    #[link_name = "rs_queue_limits_set_physical_block_size"]
    pub fn rs_queue_limits_set_physical_block_size(obj: *mut queue_limits, value: u32);
    // file_operations accessors.
    #[link_name = "rs_file_operations_owner"]
    pub fn rs_file_operations_owner(obj: *const file_operations) -> *mut module;
    #[link_name = "rs_file_operations_set_owner"]
    pub fn rs_file_operations_set_owner(obj: *mut file_operations, value: *mut module);
    #[link_name = "rs_file_operations_open"]
    pub fn rs_file_operations_open(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut inode, *mut file) -> c_int>;
    #[link_name = "rs_file_operations_set_open"]
    pub fn rs_file_operations_set_open(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut inode, *mut file) -> c_int>);
    #[link_name = "rs_file_operations_release"]
    pub fn rs_file_operations_release(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut inode, *mut file) -> c_int>;
    #[link_name = "rs_file_operations_set_release"]
    pub fn rs_file_operations_set_release(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut inode, *mut file) -> c_int>);
    #[link_name = "rs_file_operations_mmap"]
    pub fn rs_file_operations_mmap(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut file, *mut vm_area_struct) -> c_int>;
    #[link_name = "rs_file_operations_set_mmap"]
    pub fn rs_file_operations_set_mmap(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut file, *mut vm_area_struct) -> c_int>);
    #[link_name = "rs_file_operations_llseek"]
    pub fn rs_file_operations_llseek(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut file, loff_t, c_int) -> loff_t>;
    #[link_name = "rs_file_operations_set_llseek"]
    pub fn rs_file_operations_set_llseek(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut file, loff_t, c_int) -> loff_t>);
    #[link_name = "rs_file_operations_read_iter"]
    pub fn rs_file_operations_read_iter(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut kiocb, *mut iov_iter) -> isize>;
    #[link_name = "rs_file_operations_set_read_iter"]
    pub fn rs_file_operations_set_read_iter(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut kiocb, *mut iov_iter) -> isize>);
    #[link_name = "rs_file_operations_write_iter"]
    pub fn rs_file_operations_write_iter(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut kiocb, *mut iov_iter) -> isize>;
    #[link_name = "rs_file_operations_set_write_iter"]
    pub fn rs_file_operations_set_write_iter(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut kiocb, *mut iov_iter) -> isize>);
    #[link_name = "rs_file_operations_unlocked_ioctl"]
    pub fn rs_file_operations_unlocked_ioctl(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut file, c_uint, crate::ffi::c_ulong) -> c_long>;
    #[link_name = "rs_file_operations_set_unlocked_ioctl"]
    pub fn rs_file_operations_set_unlocked_ioctl(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut file, c_uint, crate::ffi::c_ulong) -> c_long>);
    #[cfg(CONFIG_COMPAT)]
    #[link_name = "rs_file_operations_compat_ioctl"]
    pub fn rs_file_operations_compat_ioctl(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut file, c_uint, crate::ffi::c_ulong) -> c_long>;
    #[cfg(CONFIG_COMPAT)]
    #[link_name = "rs_file_operations_set_compat_ioctl"]
    pub fn rs_file_operations_set_compat_ioctl(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut file, c_uint, crate::ffi::c_ulong) -> c_long>);
    #[link_name = "rs_file_operations_show_fdinfo"]
    pub fn rs_file_operations_show_fdinfo(obj: *const file_operations) -> Option<unsafe extern "C" fn(*mut seq_file, *mut file)>;
    #[link_name = "rs_file_operations_set_show_fdinfo"]
    pub fn rs_file_operations_set_show_fdinfo(obj: *mut file_operations, value: Option<unsafe extern "C" fn(*mut seq_file, *mut file)>);
    // block_device_operations accessors.
    #[link_name = "rs_block_device_operations_submit_bio"]
    pub fn rs_block_device_operations_submit_bio(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_submit_bio"]
    pub fn rs_block_device_operations_set_submit_bio(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_open"]
    pub fn rs_block_device_operations_open(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_open"]
    pub fn rs_block_device_operations_set_open(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_release"]
    pub fn rs_block_device_operations_release(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_release"]
    pub fn rs_block_device_operations_set_release(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_ioctl"]
    pub fn rs_block_device_operations_ioctl(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_ioctl"]
    pub fn rs_block_device_operations_set_ioctl(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_compat_ioctl"]
    pub fn rs_block_device_operations_compat_ioctl(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_compat_ioctl"]
    pub fn rs_block_device_operations_set_compat_ioctl(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_check_events"]
    pub fn rs_block_device_operations_check_events(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_check_events"]
    pub fn rs_block_device_operations_set_check_events(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_unlock_native_capacity"]
    pub fn rs_block_device_operations_unlock_native_capacity(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_unlock_native_capacity"]
    pub fn rs_block_device_operations_set_unlock_native_capacity(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_getgeo"]
    pub fn rs_block_device_operations_getgeo(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_getgeo"]
    pub fn rs_block_device_operations_set_getgeo(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_set_read_only"]
    pub fn rs_block_device_operations_set_read_only(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_set_read_only"]
    pub fn rs_block_device_operations_set_set_read_only(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_swap_slot_free_notify"]
    pub fn rs_block_device_operations_swap_slot_free_notify(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_swap_slot_free_notify"]
    pub fn rs_block_device_operations_set_swap_slot_free_notify(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_report_zones"]
    pub fn rs_block_device_operations_report_zones(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_report_zones"]
    pub fn rs_block_device_operations_set_report_zones(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_devnode"]
    pub fn rs_block_device_operations_devnode(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_devnode"]
    pub fn rs_block_device_operations_set_devnode(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_alternative_gpt_sector"]
    pub fn rs_block_device_operations_alternative_gpt_sector(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_alternative_gpt_sector"]
    pub fn rs_block_device_operations_set_alternative_gpt_sector(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_get_unique_id"]
    pub fn rs_block_device_operations_get_unique_id(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_get_unique_id"]
    pub fn rs_block_device_operations_set_get_unique_id(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_owner"]
    pub fn rs_block_device_operations_owner(obj: *const block_device_operations) -> *mut module;
    #[link_name = "rs_block_device_operations_set_owner"]
    pub fn rs_block_device_operations_set_owner(obj: *mut block_device_operations, value: *mut module);
    #[link_name = "rs_block_device_operations_pr_ops"]
    pub fn rs_block_device_operations_pr_ops(obj: *const block_device_operations) -> *mut c_void;
    #[link_name = "rs_block_device_operations_set_pr_ops"]
    pub fn rs_block_device_operations_set_pr_ops(obj: *mut block_device_operations, value: *mut c_void);
    #[link_name = "rs_block_device_operations_free_disk"]
    pub fn rs_block_device_operations_free_disk(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_free_disk"]
    pub fn rs_block_device_operations_set_free_disk(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_block_device_operations_poll_bio"]
    pub fn rs_block_device_operations_poll_bio(obj: *const block_device_operations) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_block_device_operations_set_poll_bio"]
    pub fn rs_block_device_operations_set_poll_bio(obj: *mut block_device_operations, value: Option<unsafe extern "C" fn(*mut c_void)>);
    // gendisk accessors.
    #[link_name = "rs_gendisk_fops"]
    pub fn rs_gendisk_fops(obj: *const gendisk) -> *const block_device_operations;
    #[link_name = "rs_gendisk_set_fops"]
    pub fn rs_gendisk_set_fops(obj: *mut gendisk, value: *const block_device_operations);
    #[link_name = "rs_gendisk_disk_name"]
    pub fn rs_gendisk_disk_name(obj: *mut gendisk) -> *mut c_char;
    // blk_mq_queue_data accessors.
    #[link_name = "rs_blk_mq_queue_data_rq"]
    pub fn rs_blk_mq_queue_data_rq(obj: *const blk_mq_queue_data) -> *mut request;
    #[link_name = "rs_blk_mq_queue_data_set_rq"]
    pub fn rs_blk_mq_queue_data_set_rq(obj: *mut blk_mq_queue_data, value: *mut request);
    #[link_name = "rs_blk_mq_queue_data_last"]
    pub fn rs_blk_mq_queue_data_last(obj: *const blk_mq_queue_data) -> bool;
    #[link_name = "rs_blk_mq_queue_data_set_last"]
    pub fn rs_blk_mq_queue_data_set_last(obj: *mut blk_mq_queue_data, value: bool);
    // blk_mq_tag_set accessors.
    #[link_name = "rs_blk_mq_tag_set_ops"]
    pub fn rs_blk_mq_tag_set_ops(obj: *const blk_mq_tag_set) -> *const blk_mq_ops;
    #[link_name = "rs_blk_mq_tag_set_set_ops"]
    pub fn rs_blk_mq_tag_set_set_ops(obj: *mut blk_mq_tag_set, value: *const blk_mq_ops);
    #[link_name = "rs_blk_mq_tag_set_nr_maps"]
    pub fn rs_blk_mq_tag_set_nr_maps(obj: *const blk_mq_tag_set) -> u32;
    #[link_name = "rs_blk_mq_tag_set_set_nr_maps"]
    pub fn rs_blk_mq_tag_set_set_nr_maps(obj: *mut blk_mq_tag_set, value: u32);
    #[link_name = "rs_blk_mq_tag_set_nr_hw_queues"]
    pub fn rs_blk_mq_tag_set_nr_hw_queues(obj: *const blk_mq_tag_set) -> u32;
    #[link_name = "rs_blk_mq_tag_set_set_nr_hw_queues"]
    pub fn rs_blk_mq_tag_set_set_nr_hw_queues(obj: *mut blk_mq_tag_set, value: u32);
    #[link_name = "rs_blk_mq_tag_set_queue_depth"]
    pub fn rs_blk_mq_tag_set_queue_depth(obj: *const blk_mq_tag_set) -> u32;
    #[link_name = "rs_blk_mq_tag_set_set_queue_depth"]
    pub fn rs_blk_mq_tag_set_set_queue_depth(obj: *mut blk_mq_tag_set, value: u32);
    #[link_name = "rs_blk_mq_tag_set_reserved_tags"]
    pub fn rs_blk_mq_tag_set_reserved_tags(obj: *const blk_mq_tag_set) -> u32;
    #[link_name = "rs_blk_mq_tag_set_set_reserved_tags"]
    pub fn rs_blk_mq_tag_set_set_reserved_tags(obj: *mut blk_mq_tag_set, value: u32);
    #[link_name = "rs_blk_mq_tag_set_cmd_size"]
    pub fn rs_blk_mq_tag_set_cmd_size(obj: *const blk_mq_tag_set) -> u32;
    #[link_name = "rs_blk_mq_tag_set_set_cmd_size"]
    pub fn rs_blk_mq_tag_set_set_cmd_size(obj: *mut blk_mq_tag_set, value: u32);
    #[link_name = "rs_blk_mq_tag_set_numa_node"]
    pub fn rs_blk_mq_tag_set_numa_node(obj: *const blk_mq_tag_set) -> c_int;
    #[link_name = "rs_blk_mq_tag_set_set_numa_node"]
    pub fn rs_blk_mq_tag_set_set_numa_node(obj: *mut blk_mq_tag_set, value: c_int);
    #[link_name = "rs_blk_mq_tag_set_timeout"]
    pub fn rs_blk_mq_tag_set_timeout(obj: *const blk_mq_tag_set) -> u32;
    #[link_name = "rs_blk_mq_tag_set_set_timeout"]
    pub fn rs_blk_mq_tag_set_set_timeout(obj: *mut blk_mq_tag_set, value: u32);
    #[link_name = "rs_blk_mq_tag_set_flags"]
    pub fn rs_blk_mq_tag_set_flags(obj: *const blk_mq_tag_set) -> u32;
    #[link_name = "rs_blk_mq_tag_set_set_flags"]
    pub fn rs_blk_mq_tag_set_set_flags(obj: *mut blk_mq_tag_set, value: u32);
    #[link_name = "rs_blk_mq_tag_set_driver_data"]
    pub fn rs_blk_mq_tag_set_driver_data(obj: *const blk_mq_tag_set) -> *mut c_void;
    #[link_name = "rs_blk_mq_tag_set_set_driver_data"]
    pub fn rs_blk_mq_tag_set_set_driver_data(obj: *mut blk_mq_tag_set, value: *mut c_void);
    // blk_mq_ops accessors.
    #[link_name = "rs_blk_mq_ops_queue_rq"]
    pub fn rs_blk_mq_ops_queue_rq(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx, *const blk_mq_queue_data) -> blk_status_t>;
    #[link_name = "rs_blk_mq_ops_set_queue_rq"]
    pub fn rs_blk_mq_ops_set_queue_rq(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx, *const blk_mq_queue_data) -> blk_status_t>);
    #[link_name = "rs_blk_mq_ops_queue_rqs"]
    pub fn rs_blk_mq_ops_queue_rqs(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut c_void)>;
    #[link_name = "rs_blk_mq_ops_set_queue_rqs"]
    pub fn rs_blk_mq_ops_set_queue_rqs(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut c_void)>);
    #[link_name = "rs_blk_mq_ops_commit_rqs"]
    pub fn rs_blk_mq_ops_commit_rqs(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx)>;
    #[link_name = "rs_blk_mq_ops_set_commit_rqs"]
    pub fn rs_blk_mq_ops_set_commit_rqs(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx)>);
    #[link_name = "rs_blk_mq_ops_get_budget"]
    pub fn rs_blk_mq_ops_get_budget(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut c_void) -> c_int>;
    #[link_name = "rs_blk_mq_ops_set_get_budget"]
    pub fn rs_blk_mq_ops_set_get_budget(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut c_void) -> c_int>);
    #[link_name = "rs_blk_mq_ops_put_budget"]
    pub fn rs_blk_mq_ops_put_budget(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut c_void, c_int)>;
    #[link_name = "rs_blk_mq_ops_set_put_budget"]
    pub fn rs_blk_mq_ops_set_put_budget(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut c_void, c_int)>);
    #[link_name = "rs_blk_mq_ops_set_rq_budget_token"]
    pub fn rs_blk_mq_ops_set_rq_budget_token(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut request, c_int)>;
    #[link_name = "rs_blk_mq_ops_set_set_rq_budget_token"]
    pub fn rs_blk_mq_ops_set_set_rq_budget_token(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut request, c_int)>);
    #[link_name = "rs_blk_mq_ops_get_rq_budget_token"]
    pub fn rs_blk_mq_ops_get_rq_budget_token(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut request) -> c_int>;
    #[link_name = "rs_blk_mq_ops_set_get_rq_budget_token"]
    pub fn rs_blk_mq_ops_set_get_rq_budget_token(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut request) -> c_int>);
    #[link_name = "rs_blk_mq_ops_timeout"]
    pub fn rs_blk_mq_ops_timeout(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut request) -> c_int>;
    #[link_name = "rs_blk_mq_ops_set_timeout"]
    pub fn rs_blk_mq_ops_set_timeout(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut request) -> c_int>);
    #[link_name = "rs_blk_mq_ops_poll"]
    pub fn rs_blk_mq_ops_poll(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx, *mut io_comp_batch) -> c_int>;
    #[link_name = "rs_blk_mq_ops_set_poll"]
    pub fn rs_blk_mq_ops_set_poll(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx, *mut io_comp_batch) -> c_int>);
    #[link_name = "rs_blk_mq_ops_complete"]
    pub fn rs_blk_mq_ops_complete(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut request)>;
    #[link_name = "rs_blk_mq_ops_set_complete"]
    pub fn rs_blk_mq_ops_set_complete(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut request)>);
    #[link_name = "rs_blk_mq_ops_init_hctx"]
    pub fn rs_blk_mq_ops_init_hctx(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx, *mut c_void, c_uint) -> c_int>;
    #[link_name = "rs_blk_mq_ops_set_init_hctx"]
    pub fn rs_blk_mq_ops_set_init_hctx(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx, *mut c_void, c_uint) -> c_int>);
    #[link_name = "rs_blk_mq_ops_exit_hctx"]
    pub fn rs_blk_mq_ops_exit_hctx(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx, c_uint)>;
    #[link_name = "rs_blk_mq_ops_set_exit_hctx"]
    pub fn rs_blk_mq_ops_set_exit_hctx(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut blk_mq_hw_ctx, c_uint)>);
    #[link_name = "rs_blk_mq_ops_init_request"]
    pub fn rs_blk_mq_ops_init_request(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut blk_mq_tag_set, *mut request, c_uint, c_uint) -> c_int>;
    #[link_name = "rs_blk_mq_ops_set_init_request"]
    pub fn rs_blk_mq_ops_set_init_request(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut blk_mq_tag_set, *mut request, c_uint, c_uint) -> c_int>);
    #[link_name = "rs_blk_mq_ops_exit_request"]
    pub fn rs_blk_mq_ops_exit_request(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut blk_mq_tag_set, *mut request, c_uint)>;
    #[link_name = "rs_blk_mq_ops_set_exit_request"]
    pub fn rs_blk_mq_ops_set_exit_request(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut blk_mq_tag_set, *mut request, c_uint)>);
    #[link_name = "rs_blk_mq_ops_cleanup_rq"]
    pub fn rs_blk_mq_ops_cleanup_rq(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut request)>;
    #[link_name = "rs_blk_mq_ops_set_cleanup_rq"]
    pub fn rs_blk_mq_ops_set_cleanup_rq(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut request)>);
    #[link_name = "rs_blk_mq_ops_busy"]
    pub fn rs_blk_mq_ops_busy(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut c_void) -> bool>;
    #[link_name = "rs_blk_mq_ops_set_busy"]
    pub fn rs_blk_mq_ops_set_busy(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut c_void) -> bool>);
    #[link_name = "rs_blk_mq_ops_map_queues"]
    pub fn rs_blk_mq_ops_map_queues(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut blk_mq_tag_set)>;
    #[link_name = "rs_blk_mq_ops_set_map_queues"]
    pub fn rs_blk_mq_ops_set_map_queues(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut blk_mq_tag_set)>);
    #[link_name = "rs_blk_mq_ops_show_rq"]
    pub fn rs_blk_mq_ops_show_rq(obj: *const blk_mq_ops) -> Option<unsafe extern "C" fn(*mut seq_file, *mut request)>;
    #[link_name = "rs_blk_mq_ops_set_show_rq"]
    pub fn rs_blk_mq_ops_set_show_rq(obj: *mut blk_mq_ops, value: Option<unsafe extern "C" fn(*mut seq_file, *mut request)>);
    // KUnit runtime initializers.
    #[link_name = "rs_kunit_case_array_init"]
    pub fn rs_kunit_case_array_init(
        cases: *mut c_void,
        index: c_uint,
        run_case: Option<unsafe extern "C" fn(*mut kunit)>,
        name: *const c_char,
    );
    #[link_name = "rs_kunit_case_array_init_null"]
    pub fn rs_kunit_case_array_init_null(cases: *mut c_void, index: c_uint);
    #[link_name = "rs_kunit_suite_init_rust"]
    pub fn rs_kunit_suite_init_rust(suite: *mut c_void, name: *const c_char, test_cases: *mut c_void);
    #[link_name = "rs_module_kunit_suites"]
    pub fn rs_module_kunit_suites(module: *mut module) -> *mut *mut kunit_suite;
    #[link_name = "rs_module_num_kunit_suites"]
    pub fn rs_module_num_kunit_suites(module: *mut module) -> c_int;
    #[link_name = "rs_kunit_register_case"]
    pub fn rs_kunit_register_case(
        module: *mut module,
        name: *const c_char,
        run_case: Option<unsafe extern "C" fn(*mut kunit)>,
    );
    #[link_name = "rs_kunit_unregister_module"]
    pub fn rs_kunit_unregister_module(module: *mut module);

    // kunit_case accessors.
    #[link_name = "rs_kunit_case_run_case"]
    pub fn rs_kunit_case_run_case(obj: *const kunit_case) -> Option<unsafe extern "C" fn(*mut kunit)>;
    #[link_name = "rs_kunit_case_set_run_case"]
    pub fn rs_kunit_case_set_run_case(obj: *mut kunit_case, value: Option<unsafe extern "C" fn(*mut kunit)>);
    #[link_name = "rs_kunit_case_name"]
    pub fn rs_kunit_case_name(obj: *const kunit_case) -> *const c_char;
    #[link_name = "rs_kunit_case_set_name"]
    pub fn rs_kunit_case_set_name(obj: *mut kunit_case, value: *const c_char);
    #[link_name = "rs_kunit_case_attr"]
    pub fn rs_kunit_case_attr(obj: *const kunit_case) -> kunit_attributes;
    #[link_name = "rs_kunit_case_set_attr"]
    pub fn rs_kunit_case_set_attr(obj: *mut kunit_case, value: kunit_attributes);
    #[link_name = "rs_kunit_case_generate_params"]
    pub fn rs_kunit_case_generate_params(obj: *const kunit_case) -> Option<unsafe extern "C" fn()>;
    #[link_name = "rs_kunit_case_set_generate_params"]
    pub fn rs_kunit_case_set_generate_params(obj: *mut kunit_case, value: Option<unsafe extern "C" fn()>);
    #[link_name = "rs_kunit_case_status"]
    pub fn rs_kunit_case_status(obj: *const kunit_case) -> kunit_status;
    #[link_name = "rs_kunit_case_set_status"]
    pub fn rs_kunit_case_set_status(obj: *mut kunit_case, value: kunit_status);
    #[link_name = "rs_kunit_case_module_name"]
    pub fn rs_kunit_case_module_name(obj: *const kunit_case) -> *mut c_char;
    #[link_name = "rs_kunit_case_set_module_name"]
    pub fn rs_kunit_case_set_module_name(obj: *mut kunit_case, value: *mut c_char);
    #[link_name = "rs_kunit_case_log"]
    pub fn rs_kunit_case_log(obj: *const kunit_case) -> *mut c_void;
    #[link_name = "rs_kunit_case_set_log"]
    pub fn rs_kunit_case_set_log(obj: *mut kunit_case, value: *mut c_void);
    #[link_name = "rs_kunit_case___kabi_reserved1"]
    pub fn rs_kunit_case___kabi_reserved1(obj: *const kunit_case) -> c_ulong;
    #[link_name = "rs_kunit_case_set___kabi_reserved1"]
    pub fn rs_kunit_case_set___kabi_reserved1(obj: *mut kunit_case, value: c_ulong);
    // kunit_suite accessors.
    #[link_name = "rs_kunit_suite_name"]
    pub fn rs_kunit_suite_name(obj: *mut kunit_suite) -> *mut c_char;
    #[link_name = "rs_kunit_suite_status_comment"]
    pub fn rs_kunit_suite_status_comment(obj: *mut kunit_suite) -> *mut c_char;
    #[link_name = "rs_kunit_suite_test_cases"]
    pub fn rs_kunit_suite_test_cases(obj: *const kunit_suite) -> *mut kunit_case;
    #[link_name = "rs_kunit_suite_set_test_cases"]
    pub fn rs_kunit_suite_set_test_cases(obj: *mut kunit_suite, value: *mut kunit_case);
    #[link_name = "rs_kunit_suite_suite_init"]
    pub fn rs_kunit_suite_suite_init(obj: *const kunit_suite) -> Option<unsafe extern "C" fn(*mut kunit_suite) -> c_int>;
    #[link_name = "rs_kunit_suite_set_suite_init"]
    pub fn rs_kunit_suite_set_suite_init(obj: *mut kunit_suite, value: Option<unsafe extern "C" fn(*mut kunit_suite) -> c_int>);
    #[link_name = "rs_kunit_suite_suite_exit"]
    pub fn rs_kunit_suite_suite_exit(obj: *const kunit_suite) -> Option<unsafe extern "C" fn(*mut kunit_suite)>;
    #[link_name = "rs_kunit_suite_set_suite_exit"]
    pub fn rs_kunit_suite_set_suite_exit(obj: *mut kunit_suite, value: Option<unsafe extern "C" fn(*mut kunit_suite)>);
    #[link_name = "rs_kunit_suite_init"]
    pub fn rs_kunit_suite_init(obj: *const kunit_suite) -> Option<unsafe extern "C" fn(*mut kunit) -> c_int>;
    #[link_name = "rs_kunit_suite_set_init"]
    pub fn rs_kunit_suite_set_init(obj: *mut kunit_suite, value: Option<unsafe extern "C" fn(*mut kunit) -> c_int>);
    #[link_name = "rs_kunit_suite_exit"]
    pub fn rs_kunit_suite_exit(obj: *const kunit_suite) -> Option<unsafe extern "C" fn(*mut kunit)>;
    #[link_name = "rs_kunit_suite_set_exit"]
    pub fn rs_kunit_suite_set_exit(obj: *mut kunit_suite, value: Option<unsafe extern "C" fn(*mut kunit)>);
    #[link_name = "rs_kunit_suite_attr"]
    pub fn rs_kunit_suite_attr(obj: *const kunit_suite) -> kunit_attributes;
    #[link_name = "rs_kunit_suite_set_attr"]
    pub fn rs_kunit_suite_set_attr(obj: *mut kunit_suite, value: kunit_attributes);
    #[link_name = "rs_kunit_suite_debugfs"]
    pub fn rs_kunit_suite_debugfs(obj: *const kunit_suite) -> *mut c_void;
    #[link_name = "rs_kunit_suite_set_debugfs"]
    pub fn rs_kunit_suite_set_debugfs(obj: *mut kunit_suite, value: *mut c_void);
    #[link_name = "rs_kunit_suite_log"]
    pub fn rs_kunit_suite_log(obj: *const kunit_suite) -> *mut c_void;
    #[link_name = "rs_kunit_suite_set_log"]
    pub fn rs_kunit_suite_set_log(obj: *mut kunit_suite, value: *mut c_void);
    #[link_name = "rs_kunit_suite_suite_init_err"]
    pub fn rs_kunit_suite_suite_init_err(obj: *const kunit_suite) -> c_int;
    #[link_name = "rs_kunit_suite_set_suite_init_err"]
    pub fn rs_kunit_suite_set_suite_init_err(obj: *mut kunit_suite, value: c_int);
    #[link_name = "rs_kunit_suite_is_init"]
    pub fn rs_kunit_suite_is_init(obj: *const kunit_suite) -> bool;
    #[link_name = "rs_kunit_suite_set_is_init"]
    pub fn rs_kunit_suite_set_is_init(obj: *mut kunit_suite, value: bool);
    #[link_name = "rs_kunit_suite___kabi_reserved1"]
    pub fn rs_kunit_suite___kabi_reserved1(obj: *const kunit_suite) -> c_ulong;
    #[link_name = "rs_kunit_suite_set___kabi_reserved1"]
    pub fn rs_kunit_suite_set___kabi_reserved1(obj: *mut kunit_suite, value: c_ulong);

    pub static mut page_shift_compat_enabled: static_key_false;
    pub static mut page_shift_compat: c_uint;
    pub static mut system_wq: *mut workqueue_struct;
    pub static mut system_highpri_wq: *mut workqueue_struct;
    pub static mut system_long_wq: *mut workqueue_struct;
    pub static mut system_unbound_wq: *mut workqueue_struct;
    pub static mut system_freezable_wq: *mut workqueue_struct;
    pub static mut system_power_efficient_wq: *mut workqueue_struct;
    pub static mut system_freezable_power_efficient_wq: *mut workqueue_struct;
}
