#!/bin/sh
# usage: apply-aliases.sh <map> <obj>...
set -e

MAP="$1"
shift

[ -f "$MAP" ] || exit 0

for OBJ in "$@"; do
	[ -f "$OBJ" ] || continue

	args=""
	while read -r long short; do
		[ -z "$long" ] && continue
		args="$args --redefine-sym $long=$short"
	done < "$MAP"

	# shellcheck disable=SC2086
	llvm-objcopy $args "$OBJ"
done
