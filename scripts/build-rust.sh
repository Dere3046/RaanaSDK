#!/bin/sh
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VER="${VER:-}"
RUSTC="${RUSTC:-rustc}"

if [ -n "$VER" ] && [ -d /opt/ddk/rust ]; then
	OUT="$ROOT/out/$VER/rust"
	RUSTC=/opt/ddk/rust/rust-1.82.0/bin/rustc
	RUST_SRC="$($RUSTC --print sysroot)/lib/rustlib/src/rust"
	CORE_SRC="$RUST_SRC/library/core/src/lib.rs"
	CB_SRC="$KDIR/source/rust/compiler_builtins.rs"

	mkdir -p "$OUT"

	"$RUSTC" --edition=2021 --crate-type proc-macro \
		--crate-name macros --extern proc_macro \
		-o "$OUT/libmacros.so" \
		"$ROOT/rust/macros/lib.rs"

	COMMON="--edition=2021 -Cpanic=abort -Cembed-bitcode=n -Clto=n"
	COMMON="$COMMON -Ccodegen-units=1 -Csymbol-mangling-version=v0"
	COMMON="$COMMON -Crelocation-model=static"
	COMMON="$COMMON --target=aarch64-unknown-none -Ctarget-feature=-neon"

	# shellcheck disable=SC2086
	"$RUSTC" $COMMON --cfg no_fp_fmt_parse \
		--crate-type rlib --crate-name core \
		--emit=obj="$OUT/core.o" --emit=metadata="$OUT/libcore.rmeta" \
		--sysroot=/dev/null "$CORE_SRC"

	llvm-objcopy \
		--redefine-sym __addsf3=__rust__addsf3 \
		--redefine-sym __eqsf2=__rust__eqsf2 \
		--redefine-sym __extendsfdf2=__rust__extendsfdf2 \
		--redefine-sym __gesf2=__rust__gesf2 \
		--redefine-sym __lesf2=__rust__lesf2 \
		--redefine-sym __ltsf2=__rust__ltsf2 \
		--redefine-sym __mulsf3=__rust__mulsf3 \
		--redefine-sym __nesf2=__rust__nesf2 \
		--redefine-sym __truncdfsf2=__rust__truncdfsf2 \
		--redefine-sym __unordsf2=__rust__unordsf2 \
		--redefine-sym __adddf3=__rust__adddf3 \
		--redefine-sym __eqdf2=__rust__eqdf2 \
		--redefine-sym __ledf2=__rust__ledf2 \
		--redefine-sym __ltdf2=__rust__ltdf2 \
		--redefine-sym __muldf3=__rust__muldf3 \
		--redefine-sym __unorddf2=__rust__unorddf2 \
		--redefine-sym __muloti4=__rust__muloti4 \
		--redefine-sym __multi3=__rust__multi3 \
		--redefine-sym __udivmodti4=__rust__udivmodti4 \
		--redefine-sym __udivti3=__rust__udivti3 \
		--redefine-sym __umodti3=__rust__umodti3 \
		--redefine-sym __ashrti3=__rust__ashrti3 \
		--redefine-sym __ashlti3=__rust__ashlti3 \
		--redefine-sym __lshrti3=__rust__lshrti3 \
		"$OUT/core.o"

	# shellcheck disable=SC2086
	"$RUSTC" $COMMON \
		--crate-type rlib --crate-name compiler_builtins \
		--extern core="$OUT/libcore.rmeta" \
		--emit=obj="$OUT/compiler_builtins.o" \
		--emit=metadata="$OUT/libcompiler_builtins.rmeta" \
		--sysroot=/dev/null "$CB_SRC"

	# shellcheck disable=SC2086
	"$RUSTC" $COMMON \
		--crate-type rlib --crate-name kernel \
		--extern core="$OUT/libcore.rmeta" \
		--extern compiler_builtins="$OUT/libcompiler_builtins.rmeta" \
		--extern macros="$OUT/libmacros.so" \
		--cfg CONFIG_MMU \
		--cfg CONFIG_PRINTK \
		--cfg CONFIG_BLOCK \
		--cfg CONFIG_64BIT \
		--emit=obj="$OUT/rust_support_rust.o" \
		--emit=metadata="$OUT/libkernel.rmeta" \
		--sysroot=/dev/null "$ROOT/rust/src/lib.rs"

	OUT="$ROOT/src/exports_rust_generated.h" MAX_SYMBOL_LEN="${MAX_SYMBOL_LEN:-0}" \
		"$ROOT/scripts/gen-exports.sh" \
		"$OUT/core.o" "$OUT/compiler_builtins.o" "$OUT/rust_support_rust.o"

	touch "$OUT/.core.o.cmd" "$OUT/.compiler_builtins.o.cmd" \
		"$OUT/.rust_support_rust.o.cmd"

	echo "-> $OUT/core.o"
	echo "-> $OUT/compiler_builtins.o"
	echo "-> $OUT/rust_support_rust.o"
else
	OUT="${OUT:-$ROOT/rust}"
	mkdir -p "$OUT"
	"$RUSTC" --edition=2021 --crate-type lib \
		-C symbol-mangling-version=v0 \
		--crate-name kernel --emit=obj \
		-o "$OUT/rust_support_rust.o" \
		"$ROOT/rust/src/lib.rs"
	echo "-> $OUT/rust_support_rust.o"
fi
