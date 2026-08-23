// SPDX-License-Identifier: GPL-2.0

//! KUnit-based macros for Rust unit tests.
//!
//! C header: [`include/kunit/test.h`](srctree/include/kunit/test.h)
//!
//! Reference: <https://docs.kernel.org/dev-tools/kunit/index.html>

use crate::{bindings, ffi::c_char, ThisModule};
use core::{ffi::c_void, fmt};

/// Prints a KUnit error-level message.
///
/// Public but hidden since it should only be used from KUnit generated code.
#[doc(hidden)]
pub fn err(args: fmt::Arguments<'_>) {
    // SAFETY: The format string is null-terminated and the `%pA` specifier matches the argument we
    // are passing.
    #[cfg(CONFIG_PRINTK)]
    unsafe {
        bindings::_printk(
            c"\x013%pA".as_ptr() as _,
            &args as *const _ as *const c_void,
        );
    }
}

/// Prints a KUnit info-level message.
///
/// Public but hidden since it should only be used from KUnit generated code.
#[doc(hidden)]
pub fn info(args: fmt::Arguments<'_>) {
    // SAFETY: The format string is null-terminated and the `%pA` specifier matches the argument we
    // are passing.
    #[cfg(CONFIG_PRINTK)]
    unsafe {
        bindings::_printk(
            c"\x016%pA".as_ptr() as _,
            &args as *const _ as *const c_void,
        );
    }
}

use macros::kunit_tests;

/// Asserts that a boolean expression is `true` at runtime.
///
/// Public but hidden since it should only be used from generated tests.
///
/// Unlike the one in `core`, this one does not panic; instead, it is mapped to the KUnit
/// facilities. See [`assert!`] for more details.
#[doc(hidden)]
#[macro_export]
macro_rules! kunit_assert {
    ($name:literal, $file:literal, $diff:expr, $condition:expr $(,)?) => {
        'out: {
            // Do nothing if the condition is `true`.
            if $condition {
                break 'out;
            }

            static FILE: &'static $crate::str::CStr = $crate::c_str!($file);
            static LINE: i32 = core::line!() as i32 - $diff;
            static CONDITION: &'static $crate::str::CStr = $crate::c_str!(stringify!($condition));

            // SAFETY: FFI call without safety requirements.
            let kunit_test = unsafe { $crate::bindings::kunit_get_current_test() };
            if kunit_test.is_null() {
                // The assertion failed but this task is not running a KUnit test, so we cannot call
                // KUnit, but at least print an error to the kernel log. This may happen if this
                // macro is called from an spawned thread in a test (see
                // `scripts/rustdoc_test_gen.rs`) or if some non-test code calls this macro by
                // mistake (it is hidden to prevent that).
                //
                // This mimics KUnit's failed assertion format.
                $crate::kunit::err(format_args!(
                    "    # {}: ASSERTION FAILED at {FILE}:{LINE}\n",
                    $name
                ));
                $crate::kunit::err(format_args!(
                    "    Expected {CONDITION} to be true, but is false\n"
                ));
                $crate::kunit::err(format_args!(
                    "    Failure not reported to KUnit since this is a non-KUnit task\n"
                ));
                break 'out;
            }

            #[repr(transparent)]
            struct Location($crate::bindings::kunit_loc);

            #[repr(transparent)]
            struct UnaryAssert($crate::bindings::kunit_unary_assert);

            // SAFETY: There is only a static instance and in that one the pointer field points to
            // an immutable C string.
            unsafe impl Sync for Location {}

            // SAFETY: There is only a static instance and in that one the pointer field points to
            // an immutable C string.
            unsafe impl Sync for UnaryAssert {}

            static LOCATION: Location = Location($crate::bindings::kunit_loc {
                file: FILE.as_char_ptr(),
                line: LINE,
            });
            static ASSERTION: UnaryAssert = UnaryAssert($crate::bindings::kunit_unary_assert {
                assert: $crate::bindings::kunit_assert {},
                condition: CONDITION.as_char_ptr(),
                expected_true: true,
            });

            // SAFETY:
            //   - FFI call.
            //   - The `kunit_test` pointer is valid because we got it from
            //     `kunit_get_current_test()` and it was not null. This means we are in a KUnit
            //     test, and that the pointer can be passed to KUnit functions and assertions.
            //   - The string pointers (`file` and `condition` above) point to null-terminated
            //     strings since they are `CStr`s.
            //   - The function pointer (`format`) points to the proper function.
            //   - The pointers passed will remain valid since they point to `static`s.
            //   - The format string is allowed to be null.
            //   - There are, however, problems with this: first of all, this will end up stopping
            //     the thread, without running destructors. While that is problematic in itself,
            //     it is considered UB to have what is effectively a forced foreign unwind
            //     with `extern "C"` ABI. One could observe the stack that is now gone from
            //     another thread. We should avoid pinning stack variables to prevent library UB,
            //     too. For the moment, given that test failures are reported immediately before the
            //     next test runs, that test failures should be fixed and that KUnit is explicitly
            //     documented as not suitable for production environments, we feel it is reasonable.
            unsafe {
                $crate::bindings::__kunit_do_failed_assertion(
                    kunit_test,
                    core::ptr::addr_of!(LOCATION.0),
                    $crate::bindings::kunit_assert_type_KUNIT_ASSERTION,
                    core::ptr::addr_of!(ASSERTION.0.assert),
                    Some($crate::bindings::kunit_unary_assert_format),
                    core::ptr::null(),
                );
            }

            // SAFETY: FFI call; the `test` pointer is valid because this hidden macro should only
            // be called by the generated documentation tests which forward the test pointer given
            // by KUnit.
            unsafe {
                $crate::bindings::__kunit_abort(kunit_test);
            }
        }
    };
}

/// Asserts that two expressions are equal to each other (using [`PartialEq`]).
///
/// Public but hidden since it should only be used from generated tests.
///
/// Unlike the one in `core`, this one does not panic; instead, it is mapped to the KUnit
/// facilities. See [`assert!`] for more details.
#[doc(hidden)]
#[macro_export]
macro_rules! kunit_assert_eq {
    ($name:literal, $file:literal, $diff:expr, $left:expr, $right:expr $(,)?) => {{
        // For the moment, we just forward to the expression assert because, for binary asserts,
        // KUnit supports only a few types (e.g. integers).
        $crate::kunit_assert!($name, $file, $diff, $left == $right);
    }};
}

/// Metadata for a single KUnit test case.
///
/// This is the Rust-side description of a test case. The actual C `struct
/// kunit_case` is stored in an opaque byte pool and filled in by C shims at
/// module init time, so the Rust source does not depend on the kernel-version
/// specific layout.
#[doc(hidden)]
pub struct KUnitCaseMeta {
    name: *const c_char,
    run_case: Option<unsafe extern "C" fn(*mut bindings::kunit)>,
}

impl KUnitCaseMeta {
    /// Creates metadata for a real test case.
    pub const fn new(
        name: *const c_char,
        run_case: unsafe extern "C" fn(*mut bindings::kunit),
    ) -> Self {
        Self {
            name,
            run_case: Some(run_case),
        }
    }

    /// Creates metadata for the NULL test-case delimiter.
    pub const fn null() -> Self {
        Self {
            name: core::ptr::null(),
            run_case: None,
        }
    }
}

/// Maximum number of test cases supported in one suite (including the NULL
/// delimiter). This is a Rust-side pool bound; the real C array is written by
/// C shims with the correct per-kernel stride.
const KUNIT_MAX_TEST_CASES: usize = 128;

/// Byte size reserved for one C `struct kunit_case` on all supported kernels.
const KUNIT_CASE_STORAGE_SIZE: usize = 64;

/// Byte size reserved for one C `struct kunit_suite` on all supported kernels.
const KUNIT_SUITE_STORAGE_SIZE: usize = 1024;

/// Opaque byte pool holding a contiguous array of C `struct kunit_case`.
///
/// Rust never indexes into this pool directly; C shims use the real
/// `sizeof(struct kunit_case)` stride.
#[doc(hidden)]
#[repr(C, align(8))]
pub struct KUnitCaseArray {
    data: [u8; KUNIT_MAX_TEST_CASES * KUNIT_CASE_STORAGE_SIZE],
}

impl KUnitCaseArray {
    /// Creates a zeroed storage pool.
    pub const fn zeroed() -> Self {
        Self {
            data: [0; KUNIT_MAX_TEST_CASES * KUNIT_CASE_STORAGE_SIZE],
        }
    }

    /// Returns a pointer to the beginning of the case pool.
    pub fn as_mut_ptr(&mut self) -> *mut c_void {
        self.data.as_mut_ptr().cast()
    }
}

/// Rust-side metadata for one KUnit suite.
#[doc(hidden)]
#[repr(C)]
struct KUnitSuiteMeta {
    name: *const c_char,
    test_cases: *mut c_void,
    case_meta: *const KUnitCaseMeta,
    num_cases: usize,
}

/// Opaque storage for a C `struct kunit_suite`, preceded by Rust-side metadata.
///
/// The `.kunit_test_suites` section entry points to `suite`; `init_test_suites`
/// uses `container_of!` to recover the metadata.
#[doc(hidden)]
#[repr(C, align(8))]
pub struct KUnitSuiteSlot {
    meta: KUnitSuiteMeta,
    pub suite: [u8; KUNIT_SUITE_STORAGE_SIZE],
}

impl KUnitSuiteSlot {
    /// Creates a zeroed suite slot with the given metadata.
    pub const fn new(
        name: *const c_char,
        test_cases: *mut c_void,
        case_meta: *const KUnitCaseMeta,
        num_cases: usize,
    ) -> Self {
        Self {
            meta: KUnitSuiteMeta {
                name,
                test_cases,
                case_meta,
                num_cases,
            },
            suite: [0; KUNIT_SUITE_STORAGE_SIZE],
        }
    }
}

/// Initializes all Rust KUnit suites registered in the current module.
///
/// This must be called from the module init path before the module reaches
/// `MODULE_STATE_LIVE`; the KUnit module notifier runs the suites after that
/// point.
pub fn init_test_suites(module: &ThisModule) {
    // SAFETY: `module` is a valid `ThisModule` pointer. The shims return NULL/0
    // when KUnit is not enabled.
    let suites = unsafe { bindings::rs_module_kunit_suites(module.as_ptr()) };
    let num = unsafe { bindings::rs_module_num_kunit_suites(module.as_ptr()) };
    if suites.is_null() || num <= 0 {
        return;
    }

    for i in 0..num as isize {
        // SAFETY: `suites` points to the module's `.kunit_test_suites` array,
        // which is valid for `num` entries.
        let suite_ptr = unsafe { *suites.offset(i) };
        if suite_ptr.is_null() {
            continue;
        }

        // SAFETY: Every pointer in this array was placed there by
        // `kunit_unsafe_test_suite!`, so it points into a `KUnitSuiteSlot`.
        let slot = unsafe { &*crate::container_of!(suite_ptr, KUnitSuiteSlot, suite) };

        let cases = slot.meta.test_cases;
        let case_meta = slot.meta.case_meta;
        for j in 0..slot.meta.num_cases {
            // SAFETY: `case_meta` points to a static array of `num_cases`
            // entries and `cases` is a byte pool large enough for them.
            let meta = unsafe { &*case_meta.add(j) };
            if meta.name.is_null() {
                // SAFETY: C helper writes the version-correct NULL terminator.
                unsafe { bindings::rs_kunit_case_array_init_null(cases, j as u32) };
            } else {
                // SAFETY: C helper writes the version-correct test case.
                unsafe {
                    bindings::rs_kunit_case_array_init(
                        cases,
                        j as u32,
                        meta.run_case,
                        meta.name,
                    )
                };
                // SAFETY: C helper stores a KCFI-safe dispatcher in the case
                // and remembers the real Rust test function in its registry.
                unsafe {
                    bindings::rs_kunit_register_case(
                        module.as_ptr(),
                        meta.name,
                        meta.run_case,
                    )
                };
            }
        }

        // SAFETY: `suite_ptr` points into the `suite` field of a
        // `KUnitSuiteSlot`, whose size is enough for any supported kernel.
        unsafe {
            bindings::rs_kunit_suite_init_rust(
                suite_ptr as *mut c_void,
                slot.meta.name,
                slot.meta.test_cases,
            )
        };
    }
}

/// C-callable entry point used by the `rust_support` LKM itself.
///
/// `module` must be the address of the module's `__this_module`.
#[no_mangle]
pub extern "C" fn rs_kunit_init_rust(module: *mut bindings::module) {
    // SAFETY: The caller passes a valid `__this_module` pointer.
    let this = unsafe { ThisModule::from_ptr(module) };
    init_test_suites(&this);
}

/// Registers a KUnit test suite.
///
/// # Safety
///
/// `test_cases` must be a NULL terminated array of valid test-case metadata,
/// whose lifetime is at least that of the test suite (i.e., static).
///
/// # Examples
///
/// ```ignore
/// extern "C" fn test_fn(_test: *mut kernel::bindings::kunit) {
///     let actual = 1 + 1;
///     let expected = 2;
///     assert_eq!(actual, expected);
/// }
///
/// static mut KUNIT_TEST_CASES: [kernel::kunit::KUnitCaseMeta; 2] = [
///     kernel::kunit::KUnitCaseMeta::new(kernel::c_str!("name").as_char_ptr(), test_fn),
///     kernel::kunit::KUnitCaseMeta::null(),
/// ];
/// kernel::kunit_unsafe_test_suite!(suite_name, KUNIT_TEST_CASES, 2);
/// ```
#[doc(hidden)]
#[macro_export]
macro_rules! kunit_unsafe_test_suite {
    ($name:ident, $test_cases:ident, $num_cases:expr) => {
        const _: () = {
            static KUNIT_TEST_SUITE_NAME: &::kernel::str::CStr =
                ::kernel::c_str!(::core::stringify!($name));

            static mut KUNIT_TEST_CASES: ::kernel::kunit::KUnitCaseArray =
                ::kernel::kunit::KUnitCaseArray::zeroed();

            static mut KUNIT_TEST_SUITE: ::kernel::kunit::KUnitSuiteSlot =
                ::kernel::kunit::KUnitSuiteSlot::new(
                    KUNIT_TEST_SUITE_NAME.as_char_ptr(),
                    unsafe {
                        ::core::ptr::addr_of_mut!(KUNIT_TEST_CASES) as *mut ::core::ffi::c_void
                    },
                    unsafe {
                        ::core::ptr::addr_of!($test_cases)
                            .cast::<::kernel::kunit::KUnitCaseMeta>()
                    },
                    $num_cases,
                );

            #[used]
            #[allow(unused_unsafe)]
            #[cfg_attr(not(target_os = "macos"), link_section = ".kunit_test_suites")]
            static mut KUNIT_TEST_SUITE_ENTRY: *const ::kernel::bindings::kunit_suite =
                // SAFETY: `KUNIT_TEST_SUITE` is a static slot whose `suite`
                // field is the actual storage for the C struct.
                unsafe { ::core::ptr::addr_of!(KUNIT_TEST_SUITE.suite).cast() };
        };
    };
}

/// Returns whether we are currently running a KUnit test.
///
/// In some cases, you need to call test-only code from outside the test case, for example, to
/// create a function mock. This function allows to change behavior depending on whether we are
/// currently running a KUnit test or not.
///
/// # Examples
///
/// This example shows how a function can be mocked to return a well-known value while testing:
///
/// ```
/// # use kernel::kunit::in_kunit_test;
/// fn fn_mock_example(n: i32) -> i32 {
///     if in_kunit_test() {
///         return 100;
///     }
///
///     n + 1
/// }
///
/// let mock_res = fn_mock_example(5);
/// assert_eq!(mock_res, 100);
/// ```
pub fn in_kunit_test() -> bool {
    // SAFETY: `kunit_get_current_test()` is always safe to call (it has fallbacks for
    // when KUnit is not enabled).
    !unsafe { bindings::kunit_get_current_test() }.is_null()
}

#[kunit_tests(rust_kernel_kunit)]
mod tests {
    use super::*;

    #[test]
    fn rust_test_kunit_example_test() {
        #![expect(clippy::eq_op)]
        assert_eq!(1 + 1, 2);
    }

    #[test]
    fn rust_test_kunit_in_kunit_test() {
        assert!(in_kunit_test());
    }
}
