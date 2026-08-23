#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

set -e

REAL_MODPOST="${REAL_MODPOST:-/opt/ddk/kdir/android16-6.12/scripts/mod/modpost}"

"$REAL_MODPOST" "$@"

if [ -n "$KBUILD_EXTMOD" ] && [ -d "$KBUILD_EXTMOD" ]; then
	SYMVERS="$KBUILD_EXTMOD/Module.symvers"
	for modc in "$KBUILD_EXTMOD"/*.mod.c; do
		[ -f "$modc" ] || continue
		modobj="${modc%.mod.c}.o"
		python3 /src/scripts/fix-modpost-modc.py \
			"$modc" "$SYMVERS" "$modobj" /src/src/exports_rust_generated.h
	done
fi

exit 0
