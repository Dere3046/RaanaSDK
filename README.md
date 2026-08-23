# RaanaSDK

Rust support LKM for Android GKI kernels.

Load `rust_support.ko` first, then load Rust LKMs built against its
exported Rust runtime. No kernel rebuild required.

## Supported targets

* android12 5.10
* android13 5.10
* android13 5.15
* android14 5.15
* android14 6.1
* android15 6.6
* android16 6.12

## Features

* Rust runtime exports: core, compiler_builtins, kernel crate
* version sensitive structs use opaque bindings
* runtime accessors via C shims and type_info
* KCFI safe module entry through C wrapper
* KUnit support with C dispatcher
* works with KMSDK and rust_lkm_template

## Build

```sh
./scripts/build-ddkk.sh android16-6.12
```

Output:

```text
out/<target>/rust_support.ko
out/<target>/Module.symvers
out/<target>/rust/libkernel.rmeta
```

## Write a Rust LKM

Use the template:

```sh
git clone git@github.com:Dere3046/rust_lkm_template.git
cd rust_lkm_template
scripts/fetch-deps.sh
scripts/build-ddkk.sh android16-6.12
```

## KUnit

Rust LKMs can use `#[kunit_tests]`.

```rust
use kernel::macros::kunit_tests;

#[kunit_tests(my_suite)]
mod tests {
    #[test]
    fn test_add() {
        assert_eq!(1 + 1, 2);
    }
}
```

## License

GPL-2.0
