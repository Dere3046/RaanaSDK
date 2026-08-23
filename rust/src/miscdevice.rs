// SPDX-License-Identifier: GPL-2.0

// Copyright (C) 2024 Google LLC.

//! Miscdevice support.
//!
//! C headers: [`include/linux/miscdevice.h`](srctree/include/linux/miscdevice.h).
//!
//! Reference: <https://www.kernel.org/doc/html/latest/driver-api/misc_devices.html>

use crate::{
    bindings,
    device::Device,
    error::{Error, Result, VTABLE_DEFAULT_ERROR},
    ffi::{c_char, c_int, c_long, c_uint, c_ulong, c_void},
    fs::File,
    mm::virt::VmaNew,
    prelude::*,
    seq_file::SeqFile,
    str::CStr,
    types::{AsBytes, ForeignOwnable, Opaque},
};
use core::{marker::PhantomData, pin::Pin, ptr::NonNull};

extern "C" {
    fn rs_file_private_data(file: *mut bindings::file) -> *mut c_void;
    fn rs_file_set_private_data(file: *mut bindings::file, data: *mut c_void);

    fn rs_miscdevice_init(
        name: *const c_char,
        fops: *const MiscdeviceFops,
        owner: *mut c_void,
    ) -> *mut bindings::miscdevice;
    fn rs_miscdevice_free(misc: *mut bindings::miscdevice);
    fn rs_miscdevice_owner(misc: *const bindings::miscdevice) -> *mut c_void;
    fn rs_miscdevice_this_device(misc: *const bindings::miscdevice) -> *mut bindings::device;

    fn rs_kiocb_ki_filp(kiocb: *const bindings::kiocb) -> *mut bindings::file;
    fn rs_kiocb_ki_pos(kiocb: *const bindings::kiocb) -> loff_t;
    fn rs_kiocb_set_ki_pos(kiocb: *mut bindings::kiocb, value: loff_t);
}

/// The kernel `loff_t` type.
#[allow(non_camel_case_types)]
pub type loff_t = bindings::loff_t;

/// Options for creating a misc device.
#[derive(Copy, Clone)]
pub struct MiscDeviceOptions {
    /// The name of the miscdevice.
    pub name: &'static CStr,
}

impl MiscDeviceOptions {
    /// Create a C `struct miscdevice` ready for registration.
    ///
    /// The C helper `rs_miscdevice_init` allocates both the `struct miscdevice` and the
    /// `struct file_operations` it points to. The returned pointer must later be released with
    /// `rs_miscdevice_free` after `misc_deregister`.
    ///
    /// # Safety
    ///
    /// `owner` must point to the [`MiscDeviceRegistration<T>`] that owns the returned miscdevice,
    /// and that object must stay alive until `rs_miscdevice_free` is called.
    pub unsafe fn into_raw<T: MiscDevice>(self, owner: *mut c_void) -> Result<*mut bindings::miscdevice> {
        let fops = MiscdeviceVTable::<T>::build();
        // SAFETY: `name` points to a NUL-terminated string that lives for the lifetime of the
        // miscdevice (as before). `fops` is a pointer to a valid `repr(C)` callback descriptor
        // which the C helper copies into the real `struct file_operations`. `owner` is valid by
        // the caller's contract.
        let misc = unsafe { rs_miscdevice_init(self.name.as_char_ptr(), &fops, owner) };
        if misc.is_null() {
            Err(ENOMEM)
        } else {
            Ok(misc)
        }
    }
}

/// A registration of a miscdevice.
///
/// # Invariants
///
/// `inner` points to a registered misc device.
#[repr(transparent)]
#[pin_data(PinnedDrop)]
pub struct MiscDeviceRegistration<T> {
    #[pin]
    inner: Opaque<*mut bindings::miscdevice>,
    _t: PhantomData<T>,
}

// SAFETY: It is allowed to call `misc_deregister` on a different thread from where you called
// `misc_register`.
unsafe impl<T> Send for MiscDeviceRegistration<T> {}
// SAFETY: All `&self` methods on this type are written to ensure that it is safe to call them in
// parallel.
unsafe impl<T> Sync for MiscDeviceRegistration<T> {}

impl<T: MiscDevice> MiscDeviceRegistration<T> {
    /// Register a misc device.
    pub fn register(opts: MiscDeviceOptions) -> impl PinInit<Self, Error> {
        try_pin_init!(Self {
            inner <- Opaque::try_ffi_init(move |slot: *mut *mut bindings::miscdevice| {
                // `MiscDeviceRegistration<T>` is `repr(transparent)` over `inner`, so the address
                // of the `inner` storage is also the address of the registration object. The
                // pin-init machinery has already allocated this storage and the object is pinned
                // after initialization, so the address is stable for as long as the registration
                // lives.
                let owner = slot.cast::<c_void>();

                // SAFETY: `owner` points to the `MiscDeviceRegistration<T>` being initialized and
                // remains valid until the C miscdevice is freed by the destructor below.
                let misc = unsafe { opts.into_raw::<T>(owner) }?;

                // SAFETY: `misc` is a fully initialized C miscdevice returned by
                // `rs_miscdevice_init`.
                let ret = unsafe { bindings::misc_register(misc) };
                if ret < 0 {
                    // SAFETY: `misc` was allocated by `rs_miscdevice_init` and was not registered.
                    unsafe { rs_miscdevice_free(misc) };
                    return Err(Error::from_errno(ret));
                }

                // SAFETY: The initializer can write to the provided `slot`, and `misc` is now a
                // registered misc device.
                // INVARIANT: If this returns `Ok(())`, then `slot` contains a pointer to a
                // registered misc device.
                unsafe { slot.write(misc) };
                Ok(())
            }),
            _t: PhantomData,
        })
    }

    /// Returns a raw pointer to the misc device.
    pub fn as_raw(&self) -> *mut bindings::miscdevice {
        // SAFETY: The type invariant guarantees that `inner` is initialized and points to a
        // registered misc device.
        unsafe { *self.inner.get() }
    }

    /// Access the `this_device` field.
    pub fn device(&self) -> &Device {
        // SAFETY: This can only be called after a successful register(), which always
        // initialises `this_device` with a valid device. Furthermore, the signature of this
        // function tells the borrow-checker that the `&Device` reference must not outlive the
        // `&MiscDeviceRegistration<T>` used to obtain it, so the last use of the reference must be
        // before the underlying `struct miscdevice` is destroyed.
        let misc = self.as_raw();
        // SAFETY: The C helper returns the valid `this_device` pointer for the registered device.
        let dev = unsafe { rs_miscdevice_this_device(misc) };
        unsafe { Device::as_ref(dev) }
    }
}

#[pinned_drop]
impl<T> PinnedDrop for MiscDeviceRegistration<T> {
    fn drop(self: Pin<&mut Self>) {
        // SAFETY: We know that the device is registered by the type invariants.
        let misc = unsafe { *self.inner.get() };
        // SAFETY: The device is registered, so `misc_deregister` must be called before freeing it.
        unsafe { bindings::misc_deregister(misc) };
        // SAFETY: `misc` was allocated by `rs_miscdevice_init` and is no longer in use.
        unsafe { rs_miscdevice_free(misc) };
    }
}

/// Trait implemented by the private data of an open misc device.
#[vtable]
pub trait MiscDevice: Sized {
    /// What kind of pointer should `Self` be wrapped in.
    type Ptr: ForeignOwnable + Send + Sync;

    /// Called when the misc device is opened.
    ///
    /// The returned pointer will be stored as the private data for the file.
    fn open(_file: &File, _misc: &MiscDeviceRegistration<Self>) -> Result<Self::Ptr>;

    /// Called when the misc device is released.
    fn release(device: Self::Ptr, _file: &File) {
        drop(device);
    }

    /// Handle for mmap.
    ///
    /// This function is invoked when a user space process invokes the `mmap` system call on
    /// `file`. The function is a callback that is part of the VMA initializer. The kernel will do
    /// initial setup of the VMA before calling this function. The function can then interact with
    /// the VMA initialization by calling methods of `vma`. If the function does not return an
    /// error, the kernel will complete initialization of the VMA according to the properties of
    /// `vma`.
    fn mmap(
        _device: <Self::Ptr as ForeignOwnable>::Borrowed<'_>,
        _file: &File,
        _vma: &VmaNew,
    ) -> Result {
        kernel::build_error(VTABLE_DEFAULT_ERROR)
    }

    /// Seeks this miscdevice.
    fn llseek(
        _device: <Self::Ptr as ForeignOwnable>::Borrowed<'_>,
        _file: &File,
        _offset: loff_t,
        _whence: c_int,
    ) -> Result<loff_t> {
        kernel::build_error(VTABLE_DEFAULT_ERROR)
    }

    /// Read from this miscdevice.
    fn read_iter(_kiocb: Kiocb<'_, Self::Ptr>, _iov: &mut IovIter) -> Result<usize> {
        kernel::build_error(VTABLE_DEFAULT_ERROR)
    }

    /// Write to this miscdevice.
    fn write_iter(_kiocb: Kiocb<'_, Self::Ptr>, _iov: &mut IovIter) -> Result<usize> {
        kernel::build_error(VTABLE_DEFAULT_ERROR)
    }

    /// Handler for ioctls.
    ///
    /// The `cmd` argument is usually manipulated using the utilties in [`kernel::ioctl`].
    ///
    /// [`kernel::ioctl`]: mod@crate::ioctl
    fn ioctl(
        _device: <Self::Ptr as ForeignOwnable>::Borrowed<'_>,
        _file: &File,
        _cmd: u32,
        _arg: usize,
    ) -> Result<isize> {
        kernel::build_error!(VTABLE_DEFAULT_ERROR)
    }

    /// Handler for ioctls.
    ///
    /// Used for 32-bit userspace on 64-bit platforms.
    ///
    /// This method is optional and only needs to be provided if the ioctl relies on structures
    /// that have different layout on 32-bit and 64-bit userspace. If no implementation is
    /// provided, then `compat_ptr_ioctl` will be used instead.
    #[cfg(CONFIG_COMPAT)]
    fn compat_ioctl(
        _device: <Self::Ptr as ForeignOwnable>::Borrowed<'_>,
        _file: &File,
        _cmd: u32,
        _arg: usize,
    ) -> Result<isize> {
        kernel::build_error!(VTABLE_DEFAULT_ERROR)
    }

    /// Show info for this fd.
    fn show_fdinfo(
        _device: <Self::Ptr as ForeignOwnable>::Borrowed<'_>,
        _m: &SeqFile,
        _file: &File,
    ) {
        kernel::build_error!(VTABLE_DEFAULT_ERROR)
    }
}

/// Wrapper for the kernel's `struct kiocb`.
///
/// The type `T` represents the private data of the file.
pub struct Kiocb<'a, T> {
    inner: NonNull<bindings::kiocb>,
    _phantom: PhantomData<&'a T>,
}

impl<'a, T: ForeignOwnable> Kiocb<'a, T> {
    /// Get the private data in this kiocb.
    pub fn private_data(&self) -> <T as ForeignOwnable>::Borrowed<'a> {
        // SAFETY: The `kiocb` lets us access the file, and the shim accesses its private data.
        let file = unsafe { rs_kiocb_ki_filp(self.inner.as_ptr()) };
        // SAFETY: The file pointer is valid for the duration of the I/O operation.
        let private = unsafe { rs_file_private_data(file) };
        // SAFETY: The kiocb has shared access to the private data.
        unsafe { <T as ForeignOwnable>::borrow(private) }
    }

    /// Gets the current value of `ki_pos`.
    pub fn ki_pos(&self) -> loff_t {
        // SAFETY: The `kiocb` can access `ki_pos`.
        unsafe { rs_kiocb_ki_pos(self.inner.as_ptr()) }
    }

    /// Sets the `ki_pos` field.
    pub fn set_ki_pos(&mut self, value: loff_t) {
        // SAFETY: The `kiocb` can access `ki_pos`.
        unsafe { rs_kiocb_set_ki_pos(self.inner.as_ptr(), value) }
    }
}

/// Wrapper for the kernel's `struct iov_iter`.
pub struct IovIter {
    inner: Opaque<bindings::iov_iter>,
}

impl IovIter {
    /// Gets a raw pointer to the contents.
    pub fn as_raw(&self) -> *mut bindings::iov_iter {
        self.inner.get()
    }

    /// Copy bytes from this iterator.
    pub fn copy_from_iter(&mut self, buf: &mut [u8]) -> usize {
        // SAFETY: The local variable `out` is valid for writing `size_of::<T>()` bytes.
        unsafe {
            bindings::_copy_from_iter(
                buf.as_mut_ptr().cast::<c_void>(),
                buf.len(),
                self.inner.get(),
            )
        }
    }

    /// Copy bytes to this iterator.
    pub fn copy_to_iter<T: AsBytes>(&mut self, value: &T) -> Result<()> {
        let len = size_of::<T>();
        // SAFETY: The reference points to a value of type `T`, so it is valid for reading
        // `size_of::<T>()` bytes.
        let res = unsafe {
            bindings::_copy_to_iter((value as *const T).cast::<c_void>(), len, self.inner.get())
        };
        if res == len {
            Ok(())
        } else {
            Err(EFAULT)
        }
    }
}

/// Callback descriptor handed to the C builder.
///
/// This is deliberately not `bindings::file_operations`: the C side owns the layout of the real
/// `struct file_operations` and copies these callbacks into it.
#[repr(C)]
struct MiscdeviceFops {
    open: Option<unsafe extern "C" fn(*mut bindings::inode, *mut bindings::file) -> c_int>,
    release: Option<unsafe extern "C" fn(*mut bindings::inode, *mut bindings::file) -> c_int>,
    mmap: Option<unsafe extern "C" fn(*mut bindings::file, *mut bindings::vm_area_struct) -> c_int>,
    llseek: Option<unsafe extern "C" fn(*mut bindings::file, loff_t, c_int) -> loff_t>,
    read_iter: Option<unsafe extern "C" fn(*mut bindings::kiocb, *mut bindings::iov_iter) -> isize>,
    write_iter: Option<unsafe extern "C" fn(*mut bindings::kiocb, *mut bindings::iov_iter) -> isize>,
    unlocked_ioctl:
        Option<unsafe extern "C" fn(*mut bindings::file, c_uint, c_ulong) -> c_long>,
    #[cfg(CONFIG_COMPAT)]
    compat_ioctl:
        Option<unsafe extern "C" fn(*mut bindings::file, c_uint, c_ulong) -> c_long>,
    show_fdinfo: Option<unsafe extern "C" fn(*mut bindings::seq_file, *mut bindings::file)>,
}

/// A vtable for the file operations of a Rust miscdevice.
struct MiscdeviceVTable<T: MiscDevice>(PhantomData<T>);

impl<T: MiscDevice> MiscdeviceVTable<T> {
    /// # Safety
    ///
    /// `file` and `inode` must be the file and inode for a file that is undergoing initialization.
    /// The file must be associated with a `MiscDeviceRegistration<T>`.
    unsafe extern "C" fn open(inode: *mut bindings::inode, raw_file: *mut bindings::file) -> c_int {
        // SAFETY: The pointers are valid and for a file being opened.
        let ret = unsafe { bindings::generic_file_open(inode, raw_file) };
        if ret != 0 {
            return ret;
        }

        // SAFETY: The open call of a file can access the private data.
        let misc_ptr = unsafe { rs_file_private_data(raw_file) };

        // SAFETY: This is a miscdevice, so `misc_open()` set the private data to a pointer to the
        // associated `struct miscdevice` before calling into this method. The C builder stores the
        // Rust registration pointer next to the miscdevice, and `rs_miscdevice_owner` returns it.
        // Furthermore, `misc_open()` ensures that the miscdevice can't be unregistered and freed
        // during this call to `fops_open`.
        let owner = unsafe { rs_miscdevice_owner(misc_ptr.cast::<bindings::miscdevice>()) };
        let misc = unsafe { &*owner.cast::<MiscDeviceRegistration<T>>() };

        // SAFETY:
        // * This underlying file is valid for (much longer than) the duration of `T::open`.
        // * There is no active fdget_pos region on the file on this thread.
        let file = unsafe { File::from_raw_file(raw_file) };

        let ptr = match T::open(file, misc) {
            Ok(ptr) => ptr,
            Err(err) => return err.to_errno(),
        };

        // This overwrites the private data with the value specified by the user, changing the type
        // of this file's private data. All future accesses to the private data is performed by
        // other fops_* methods in this file, which all correctly cast the private data to the new
        // type.
        //
        // SAFETY: The open call of a file can access the private data.
        unsafe { rs_file_set_private_data(raw_file, ptr.into_foreign().cast_mut()) };

        0
    }

    /// # Safety
    ///
    /// `file` and `inode` must be the file and inode for a file that is being released. The file
    /// must be associated with a `MiscDeviceRegistration<T>`.
    unsafe extern "C" fn release(_inode: *mut bindings::inode, file: *mut bindings::file) -> c_int {
        // SAFETY: The release call of a file owns the private data.
        let private = unsafe { rs_file_private_data(file) };
        // SAFETY: The release call of a file owns the private data.
        let ptr = unsafe { <T::Ptr as ForeignOwnable>::from_foreign(private) };

        // SAFETY:
        // * The file is valid for the duration of this call.
        // * There is no active fdget_pos region on the file on this thread.
        T::release(ptr, unsafe { File::from_raw_file(file) });

        0
    }

    /// # Safety
    ///
    /// `file` must be a valid file that is associated with a `MiscDeviceRegistration<T>`.
    /// `vma` must be a vma that is currently being mmap'ed with this file.
    unsafe extern "C" fn mmap(
        file: *mut bindings::file,
        vma: *mut bindings::vm_area_struct,
    ) -> c_int {
        // SAFETY: The mmap call of a file can access the private data.
        let private = unsafe { rs_file_private_data(file) };
        // SAFETY: This is a Rust Miscdevice, so we call `into_foreign` in `open` and
        // `from_foreign` in `release`, and `fops_mmap` is guaranteed to be called between those
        // two operations.
        let device = unsafe { <T::Ptr as ForeignOwnable>::borrow(private) };
        // SAFETY: The caller provides a vma that is undergoing initial VMA setup.
        let area = unsafe { VmaNew::from_raw(vma) };
        // SAFETY:
        // * The file is valid for the duration of this call.
        // * There is no active fdget_pos region on the file on this thread.
        let file = unsafe { File::from_raw_file(file) };

        match T::mmap(device, file, area) {
            Ok(()) => 0,
            Err(err) => err.to_errno(),
        }
    }

    /// # Safety
    ///
    /// `file` must be a valid file that is associated with a `MiscDeviceRegistration<T>`.
    unsafe extern "C" fn llseek(
        file: *mut bindings::file,
        offset: loff_t,
        whence: c_int,
    ) -> loff_t {
        // SAFETY: The release call of a file owns the private data.
        let private = unsafe { rs_file_private_data(file) };
        // SAFETY: Ioctl calls can borrow the private data of the file.
        let device = unsafe { <T::Ptr as ForeignOwnable>::borrow(private) };
        // SAFETY:
        // * The file is valid for the duration of this call.
        // * We are inside an fdget_pos region, so there cannot be any active fdget_pos regions on
        //   other threads.
        let file = unsafe { File::from_raw_file(file) };

        match T::llseek(device, file, offset, whence) {
            Ok(res) => res as loff_t,
            Err(err) => err.to_errno() as loff_t,
        }
    }

    /// # Safety
    ///
    /// Arguments must be valid.
    unsafe extern "C" fn read_iter(
        kiocb: *mut bindings::kiocb,
        iter: *mut bindings::iov_iter,
    ) -> isize {
        let kiocb = Kiocb {
            inner: unsafe { NonNull::new_unchecked(kiocb) },
            _phantom: PhantomData,
        };
        let iov = unsafe { &mut *iter.cast::<IovIter>() };

        match T::read_iter(kiocb, iov) {
            Ok(res) => res as isize,
            Err(err) => err.to_errno() as isize,
        }
    }

    /// # Safety
    ///
    /// Arguments must be valid.
    unsafe extern "C" fn write_iter(
        kiocb: *mut bindings::kiocb,
        iter: *mut bindings::iov_iter,
    ) -> isize {
        let kiocb = Kiocb {
            inner: unsafe { NonNull::new_unchecked(kiocb) },
            _phantom: PhantomData,
        };
        let iov = unsafe { &mut *iter.cast::<IovIter>() };

        match T::write_iter(kiocb, iov) {
            Ok(res) => res as isize,
            Err(err) => err.to_errno() as isize,
        }
    }

    /// # Safety
    ///
    /// `file` must be a valid file that is associated with a `MiscDeviceRegistration<T>`.
    unsafe extern "C" fn ioctl(file: *mut bindings::file, cmd: c_uint, arg: c_ulong) -> c_long {
        // SAFETY: The ioctl call of a file can access the private data.
        let private = unsafe { rs_file_private_data(file) };
        // SAFETY: Ioctl calls can borrow the private data of the file.
        let device = unsafe { <T::Ptr as ForeignOwnable>::borrow(private) };

        // SAFETY:
        // * The file is valid for the duration of this call.
        // * There is no active fdget_pos region on the file on this thread.
        let file = unsafe { File::from_raw_file(file) };

        match T::ioctl(device, file, cmd, arg as usize) {
            Ok(ret) => ret as c_long,
            Err(err) => err.to_errno() as c_long,
        }
    }

    /// # Safety
    ///
    /// `file` must be a valid file that is associated with a `MiscDeviceRegistration<T>`.
    #[cfg(CONFIG_COMPAT)]
    unsafe extern "C" fn compat_ioctl(
        file: *mut bindings::file,
        cmd: c_uint,
        arg: c_ulong,
    ) -> c_long {
        // SAFETY: The compat ioctl call of a file can access the private data.
        let private = unsafe { rs_file_private_data(file) };
        // SAFETY: Ioctl calls can borrow the private data of the file.
        let device = unsafe { <T::Ptr as ForeignOwnable>::borrow(private) };

        // SAFETY:
        // * The file is valid for the duration of this call.
        // * There is no active fdget_pos region on the file on this thread.
        let file = unsafe { File::from_raw_file(file) };

        match T::compat_ioctl(device, file, cmd, arg as usize) {
            Ok(ret) => ret as c_long,
            Err(err) => err.to_errno() as c_long,
        }
    }

    /// # Safety
    ///
    /// - `file` must be a valid file that is associated with a `MiscDeviceRegistration<T>`.
    /// - `seq_file` must be a valid `struct seq_file` that we can write to.
    unsafe extern "C" fn show_fdinfo(seq_file: *mut bindings::seq_file, file: *mut bindings::file) {
        // SAFETY: The release call of a file owns the private data.
        let private = unsafe { rs_file_private_data(file) };
        // SAFETY: Ioctl calls can borrow the private data of the file.
        let device = unsafe { <T::Ptr as ForeignOwnable>::borrow(private) };
        // SAFETY:
        // * The file is valid for the duration of this call.
        // * There is no active fdget_pos region on the file on this thread.
        let file = unsafe { File::from_raw_file(file) };
        // SAFETY: The caller ensures that the pointer is valid and exclusive for the duration in
        // which this method is called.
        let m = unsafe { SeqFile::from_raw(seq_file) };

        T::show_fdinfo(device, m, file);
    }

    fn build() -> MiscdeviceFops {
        MiscdeviceFops {
            open: Some(Self::open),
            release: Some(Self::release),
            mmap: if T::HAS_MMAP { Some(Self::mmap) } else { None },
            llseek: if T::HAS_LLSEEK {
                Some(Self::llseek)
            } else {
                None
            },
            read_iter: if T::HAS_READ_ITER {
                Some(Self::read_iter)
            } else {
                None
            },
            write_iter: if T::HAS_WRITE_ITER {
                Some(Self::write_iter)
            } else {
                None
            },
            unlocked_ioctl: if T::HAS_IOCTL {
                Some(Self::ioctl)
            } else {
                None
            },
            #[cfg(CONFIG_COMPAT)]
            compat_ioctl: if T::HAS_COMPAT_IOCTL {
                Some(Self::compat_ioctl)
            } else if T::HAS_IOCTL {
                Some(bindings::compat_ptr_ioctl)
            } else {
                None
            },
            show_fdinfo: if T::HAS_SHOW_FDINFO {
                Some(Self::show_fdinfo)
            } else {
                None
            },
        }
    }
}
