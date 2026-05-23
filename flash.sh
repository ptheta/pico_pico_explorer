#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

UF2="build/hello_world.uf2"
if [[ ! -f "$UF2" ]]; then
    echo "error: $UF2 not found — run: cmake --build build"
    exit 1
fi

# Try to trigger reboot to BOOTSEL via the 1200 baud trick
PORT=$(ls /dev/ttyACM* 2>/dev/null | head -1 || true)
if [[ -n "$PORT" ]]; then
    echo "==> Resetting Pico via $PORT (1200 baud trick)..."
    stty -F "$PORT" 1200 2>/dev/null || true
else
    echo "==> No USB serial port found — make sure the Pico is plugged in"
    echo "    If not already in BOOTSEL mode, hold BOOTSEL and replug"
fi

# Wait for RPI-RP2 to appear (up to 10 seconds)
echo "==> Waiting for RPI-RP2 to mount..."
MOUNT=""
for i in $(seq 10); do
    MOUNT=$(lsblk -o MOUNTPOINT,LABEL -rn 2>/dev/null | awk '/RPI-RP2/{print $1}' | head -1 || true)
    [[ -n "$MOUNT" ]] && break
    sleep 1
done

if [[ -z "$MOUNT" ]]; then
    echo "error: RPI-RP2 did not mount — try holding BOOTSEL while plugging in"
    exit 1
fi

echo "==> Flashing $UF2 to $MOUNT"
cp "$UF2" "$MOUNT/"
echo "==> Done"
