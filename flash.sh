#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

ELF="build/hello_world.elf"
if [[ ! -f "$ELF" ]]; then
    echo "error: $ELF not found — run: cmake --build build"
    exit 1
fi

echo "==> Flashing via debug probe..."
openocd \
    -f interface/cmsis-dap.cfg \
    -f target/rp2040.cfg \
    -c "adapter speed 5000" \
    -c "program $ELF verify reset exit"
echo "==> Done"
