#!/bin/sh
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${OUT:-$ROOT/src/exports_rust_generated.h}"
ALIAS_MAP="${ALIAS_MAP:-$ROOT/src/rust_sym_map.txt}"
SHORT_ALIAS="${SHORT_ALIAS:-0}"

if [ "$#" -eq 0 ]; then
	echo "// empty" > "$OUT"
	if [ "$SHORT_ALIAS" = "1" ]; then
		: > "$ALIAS_MAP"
	fi
	echo "-> $OUT"
	exit 0
fi

SKIP_RUST_FMT="${SKIP_RUST_FMT:-0}"
MAX="${MAX_SYMBOL_LEN:-0}"

: > "$OUT"
if [ "$SHORT_ALIAS" = "1" ]; then
	: > "$ALIAS_MAP"
fi

TMP="$ROOT/.gen_exports.$$"
: > "$TMP"

for OBJ in "$@"; do
	if [ ! -f "$OBJ" ]; then
		echo "// missing $OBJ" >> "$OUT"
		continue
	fi
	nm -p --defined-only "$OBJ" |
		awk -v max="$MAX" -v skip_fmt="$SKIP_RUST_FMT" '
			$2 ~ /^[TRDB]$/ &&
			$3 !~ /^__cfi/ &&
			$3 !~ /^__odr_asan/ &&
			$3 !~ /^rust_helper_/ &&
			(skip_fmt == 0 || $3 != "rust_fmt_argument") &&
			(max == 0 || length($3) <= max) {
				print $3
			}' >> "$TMP"
done

sort -u "$TMP" > "$TMP.sorted"
rm -f "$TMP"

if [ "$SHORT_ALIAS" = "1" ]; then
	i=0
	while read -r sym; do
		[ -z "$sym" ] && continue
		case "$sym" in
		_R*)
			i=$((i+1))
			alias=$(printf 'rs_%06d' "$i")
			printf 'EXPORT_SYMBOL_RUST_GPL(%s);\n' "$alias" >> "$OUT"
			printf '%s %s\n' "$sym" "$alias" >> "$ALIAS_MAP"
			;;
		*)
			printf 'EXPORT_SYMBOL_RUST_GPL(%s);\n' "$sym" >> "$OUT"
			;;
		esac
	done < "$TMP.sorted"
else
	while read -r sym; do
		[ -z "$sym" ] && continue
		printf 'EXPORT_SYMBOL_RUST_GPL(%s);\n' "$sym" >> "$OUT"
	done < "$TMP.sorted"
fi

rm -f "$TMP.sorted"
echo "-> $OUT"
if [ "$SHORT_ALIAS" = "1" ]; then
	echo "-> $ALIAS_MAP"
fi
