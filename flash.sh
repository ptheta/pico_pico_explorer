#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

PICOTOOL=$(find build -name picotool -executable -type f 2>/dev/null | head -1)
if [[ -z "$PICOTOOL" ]]; then
    echo "error: picotool not found — run setup.sh first to build the project"
    exit 1
fi

ELF="build/hello_world.elf"
if [[ ! -f "$ELF" ]]; then
    echo "error: $ELF not found — run: cmake --build build"
    exit 1
fi

echo "==> Rebooting Pico into BOOTSEL mode..."
"$PICOTOOL" reboot -f 2>/dev/null || true
sleep 2

echo "==> Flashing $ELF"
"$PICOTOOL" load -x "$ELF"

echo "==> Done"
