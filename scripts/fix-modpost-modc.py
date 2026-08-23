#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""
fix <module>.mod.c
Usage:
  fix-modpost-modc.py <module.mod.c> <Module.symvers> <module.o> [exports_rust_generated.h]
"""

import os
import re
import subprocess
import sys


def load_clean_symvers(path: str) -> dict:
    """Return {symbol: (crc, gpl, namespace, module_path)} for clean lines."""
    info = {}
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for line in f:
            if "\x00" in line:
                continue
            parts = line.rstrip("\n").split("\t")
            if len(parts) < 4:
                continue
            crc, sym, mod, exp = parts[0], parts[1], parts[2], parts[3]
            ns = parts[4] if len(parts) > 4 else ""
            gpl = "_gpl" if exp == "EXPORT_SYMBOL_GPL" else ""
            info[sym] = (crc, gpl, ns, mod)
    return info


def load_rust_exports(path: str) -> list:
    """Return Rust export symbol names from exports_rust_generated.h."""
    names = []
    if not path:
        return names
    try:
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            for line in f:
                m = re.match(r"EXPORT_SYMBOL_RUST_GPL\(([^)]+)\);", line.strip())
                if m:
                    names.append(m.group(1))
    except OSError:
        pass
    return names


def load_symbol_types(obj_path: str) -> dict:
    """Return {symbol: 'FUNC'|'DATA'} from nm on the linked module object."""
    types = {}
    if not obj_path:
        return types
    try:
        out = subprocess.check_output(
            ["nm", "-p", "--defined-only", obj_path], text=True, errors="replace"
        )
    except (OSError, subprocess.CalledProcessError):
        return types
    for line in out.splitlines():
        parts = line.split()
        if len(parts) >= 3:
            sym = parts[2]
            kind = parts[1]
            if kind in ("T", "t"):
                types[sym] = "FUNC"
            elif kind in ("D", "d", "B", "b", "R", "r"):
                types[sym] = "DATA"
    return types


def main() -> int:
    if len(sys.argv) < 4:
        print(
            f"usage: {sys.argv[0]} <module.mod.c> <Module.symvers> <module.o> "
            "[exports_rust_generated.h]",
            file=sys.stderr,
        )
        return 2

    modc_path = sys.argv[1]
    symvers_path = sys.argv[2]
    obj_path = sys.argv[3]
    exports_header = sys.argv[4] if len(sys.argv) > 4 else ""

    with open(modc_path, "rb") as f:
        data = f.read()

    text = data.replace(b"\x00", b"").decode("utf-8", "replace")

    starts = [pos for pos in (text.find("KSYMTAB_"), text.find("SYMBOL_CRC(")) if pos != -1]
    if not starts:
        return 0
    start = min(starts)
    end_marker = "static const struct modversion_info"
    end = text.find(end_marker)
    if end == -1:
        return 0

    old_region = text[start:end]
    has_ksymtab = "KSYMTAB_" in old_region
    has_crc = "SYMBOL_CRC(" in old_region

    # Recover FUNC/DATA from the corrupted KSYMTAB tokens as a fallback.
    ksym_types = {}
    for m in re.finditer(r"KSYMTAB_(FUNC|DATA)\(([A-Za-z0-9_]+)", old_region):
        ksym_types[m.group(2)] = m.group(1)

    # Recover real CRCs from the corrupted SYMBOL_CRC lines. For long Rust
    # symbols the full symbol name fits before modpost's 500-byte truncation,
    # so this is more reliable than Module.symvers.
    modc_crcs = {}
    for m in re.finditer(r"SYMBOL_CRC\(([A-Za-z0-9_]+),\s*(0x[0-9a-fA-F]+)", old_region):
        modc_crcs[m.group(1)] = m.group(2)

    sym_info = load_clean_symvers(symvers_path)

    # Module path for Rust-only entries reconstructed from the mod.c location.
    default_mod = os.path.splitext(os.path.basename(modc_path))[0]
    default_mod_path = os.path.join(os.path.dirname(modc_path), default_mod)

    # Long Rust symbols may be missing/corrupted in Module.symvers; add them
    # from the authoritative generated Rust export header.
    for sym in load_rust_exports(exports_header):
        if sym not in sym_info:
            sym_info[sym] = ("0x7ec472ba", "_gpl", "", default_mod_path)

    # Prefer CRCs recovered from mod.c over the (possibly dummy) header values.
    for sym, crc in modc_crcs.items():
        if sym in sym_info:
            crc_old, gpl, ns, mod = sym_info[sym]
            sym_info[sym] = (crc, gpl, ns, mod)

    # Prefer nm classification, fall back to KSYMTAB token classification.
    nm_types = load_symbol_types(obj_path)
    for sym in sym_info:
        if sym not in nm_types and sym in ksym_types:
            nm_types[sym] = ksym_types[sym]

    ksym_lines = []
    symvers_lines = []
    for sym, (crc, gpl, ns, mod) in sym_info.items():
        typ = nm_types.get(sym, "FUNC")
        if has_ksymtab:
            ksym_lines.append(f'KSYMTAB_{typ}({sym}, "{gpl}", "{ns}");')
        export_kind = "EXPORT_SYMBOL_GPL" if gpl == "_gpl" else "EXPORT_SYMBOL"
        symvers_lines.append(f"{crc}\t{sym}\t{mod}\t{export_kind}\t{ns}")

    block = ""
    if has_ksymtab:
        block += "\n".join(ksym_lines) + "\n\n"

    if has_crc:
        crc_lines = [
            f'SYMBOL_CRC({sym}, {crc}, "{gpl}");'
            for sym, (crc, gpl, _ns, _mod) in sym_info.items()
        ]
        block += "\n".join(crc_lines) + "\n\n"

    new_text = text[:start] + block + text[end:]

    with open(modc_path, "w", encoding="utf-8") as f:
        f.write(new_text)

    with open(symvers_path, "w", encoding="utf-8") as f:
        f.write("\n".join(symvers_lines) + "\n")

    return 0


if __name__ == "__main__":
    sys.exit(main())
