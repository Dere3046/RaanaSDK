#!/bin/sh
# usage: build-ddkk.sh <target>
set -e

TARGET=${1:-android16-6.12}
IMAGE=docker.cnb.cool/ylarod/ddk/ddk-min:${TARGET}
RUST_IMAGE=docker.cnb.cool/ylarod/ddk/ddk-min:android16-6.12
SRCDIR=$(cd "$(dirname "$0")/.." && pwd)

# old GKI loaders reject long Rust v0 exports
# newer targets use full exports via modpost fix
case "$TARGET" in
	android12-5.10|android13-5.10|android13-5.15|android14-5.15)
		SHORT_ALIAS=${SHORT_ALIAS:-1}
		if [ "$SHORT_ALIAS" = "1" ]; then
			MAX_SYMBOL_LEN=0
		else
			MAX_SYMBOL_LEN=${MAX_SYMBOL_LEN:-400}
		fi
		;;
	*)
		MAX_SYMBOL_LEN=0
		SHORT_ALIAS=${SHORT_ALIAS:-0}
		;;
esac

# real 6.12 GKI has CONFIG_RUST=y and owns rust_fmt_argument
case "$TARGET" in
	android16-6.12)
		SKIP_RUST_FMT=${SKIP_RUST_FMT:-1}
		;;
	*)
		SKIP_RUST_FMT=${SKIP_RUST_FMT:-0}
		;;
esac

# real 6.12 GKI owns rust_helper_* exports
# skip to avoid duplicate
case "$TARGET" in
	android16-6.12)
		SKIP_RUST_HELPERS=${SKIP_RUST_HELPERS:-1}
		;;
	*)
		SKIP_RUST_HELPERS=${SKIP_RUST_HELPERS:-0}
		;;
esac

echo "== clean ($TARGET) =="
docker run --rm \
	-e KDIR=/opt/ddk/kdir/${TARGET} \
	-e VER=${TARGET} \
	-v "$SRCDIR":/src \
	-w /src \
	"$IMAGE" \
	sh -c 'make clean VER="$1"' sh "$TARGET" 2>/dev/null || true

echo "== build rust (generic arm64) =="
docker run --rm \
	-e KDIR=/opt/ddk/kdir/android16-6.12 \
	-e VER=${TARGET} \
	-e MAX_SYMBOL_LEN=${MAX_SYMBOL_LEN} \
	-e SKIP_RUST_FMT=${SKIP_RUST_FMT} \
	-v "$SRCDIR":/src \
	-w /src \
	"$RUST_IMAGE" \
	sh -c 'scripts/build-rust.sh'

echo "== regenerate exports for $TARGET =="
docker run --rm \
	-e KDIR=/opt/ddk/kdir/${TARGET} \
	-e VER=${TARGET} \
	-e MAX_SYMBOL_LEN=${MAX_SYMBOL_LEN} \
	-e SKIP_RUST_FMT=${SKIP_RUST_FMT} \
	-e SHORT_ALIAS=${SHORT_ALIAS} \
	-e ALIAS_MAP=/src/out/${TARGET}/rust_sym_map.txt \
	-v "$SRCDIR":/src \
	-w /src \
	"$IMAGE" \
	sh -c 'OUT=src/exports_rust_generated.h MAX_SYMBOL_LEN="$1" scripts/gen-exports.sh out/$2/rust/core.o out/$2/rust/compiler_builtins.o out/$2/rust/rust_support_rust.o' sh "$MAX_SYMBOL_LEN" "$TARGET"

if [ "$SHORT_ALIAS" = "1" ]; then
	echo "== apply short aliases to rust objects =="
	docker run --rm \
		-v "$SRCDIR":/src \
		-w /src \
		"$RUST_IMAGE" \
		sh -c 'scripts/apply-aliases.sh out/$1/rust_sym_map.txt out/$1/rust/core.o out/$1/rust/compiler_builtins.o out/$1/rust/rust_support_rust.o' sh "$TARGET"
fi

echo "== build C + link ($TARGET) =="
# 6.1+ uses modpost fix wrapper
# 5.10 5.15 keeps normal path
MODPOST_ARG=""
case "$TARGET" in
	android14-6.1|android15-6.6|android16-6.12)
		MODPOST_ARG="MODPOST=/src/scripts/fix-modpost-wrapper.sh"
		;;
esac
docker run --rm \
	-e KDIR=/opt/ddk/kdir/${TARGET} \
	-e VER=${TARGET} \
	-e REAL_MODPOST=/opt/ddk/kdir/${TARGET}/scripts/mod/modpost \
	-e SKIP_RUST_HELPERS=${SKIP_RUST_HELPERS} \
	-v "$SRCDIR":/src \
	-w /src \
	"$IMAGE" \
	sh -c 'make VER="$1" '"$MODPOST_ARG" sh "$TARGET"

echo "-> ${SRCDIR}/out/${TARGET}"
find "$SRCDIR/out/${TARGET}" -name "*.ko"
