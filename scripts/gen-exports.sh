#!/bin/sh
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${OUT:-$ROOT/src/exports_rust_generated.h}"

if [ "$#" -eq 0 ]; then
	echo "// empty" > "$OUT"
	echo "-> $OUT"
	exit 0
fi

SKIP_RUST_FMT="${SKIP_RUST_FMT:-0}"
if [ "$SKIP_RUST_FMT" = "0" ] && [ -n "$KDIR" ] &&
	grep -q "CONFIG_RUST=y" "$KDIR/include/generated/autoconf.h" 2>/dev/null; then
	SKIP_RUST_FMT=1
fi

: > "$OUT"
for OBJ in "$@"; do
	if [ ! -f "$OBJ" ]; then
		echo "// missing $OBJ" >> "$OUT"
		continue
	fi
	MAX="${MAX_SYMBOL_LEN:-0}"
	nm -p --defined-only "$OBJ" |
		awk -v max="$MAX" -v skip_fmt="$SKIP_RUST_FMT" '
			$2 ~ /^[TRDB]$/ &&
			$3 !~ /^__cfi/ &&
			$3 !~ /^__odr_asan/ &&
			$3 !~ /^rust_helper_/ &&
			(skip_fmt == 0 || $3 != "rust_fmt_argument") &&
			(max == 0 || length($3) <= max) {
				printf "EXPORT_SYMBOL_RUST_GPL(%s);\n", $3
			}' >> "$OUT"
done

echo "-> $OUT"
