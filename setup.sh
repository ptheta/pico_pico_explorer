#!/usr/bin/env bash
# Sets up the build environment for pico_exp1 and compiles the project.
# Run once from the pico_exp1 directory: bash setup.sh

set -euo pipefail
cd "$(dirname "$0")"

PICO_DIR="$HOME/pico"

echo "==> Creating $PICO_DIR"
mkdir -p "$PICO_DIR"

# --- toolchain & cmake ---
echo "==> Installing ARM toolchain and build tools"
sudo dnf install -y \
    cmake \
    make \
    gcc \
    gcc-c++ \
    arm-none-eabi-gcc-cs \
    arm-none-eabi-gcc-cs-c++ \
    arm-none-eabi-binutils-cs \
    arm-none-eabi-newlib \
    libusb1-devel \
    git \
    python3

# --- pico-sdk ---
if [ ! -d "$PICO_DIR/pico-sdk" ]; then
    echo "==> Cloning pico-sdk"
    git clone --depth 1 https://github.com/raspberrypi/pico-sdk.git "$PICO_DIR/pico-sdk"
    git -C "$PICO_DIR/pico-sdk" submodule update --init --depth 1
else
    echo "==> pico-sdk already present"
fi
export PICO_SDK_PATH="$PICO_DIR/pico-sdk"

# --- pimoroni-pico ---
if [ ! -d "$PICO_DIR/pimoroni-pico" ]; then
    echo "==> Cloning pimoroni-pico"
    git clone --depth 1 https://github.com/pimoroni/pimoroni-pico.git "$PICO_DIR/pimoroni-pico"
else
    echo "==> pimoroni-pico already present"
fi
export PIMORONI_PICO_PATH="$PICO_DIR/pimoroni-pico"

# Copy pico_sdk_import.cmake locally so CMake can find it without the env var
cp "$PICO_SDK_PATH/external/pico_sdk_import.cmake" .

# --- build ---
echo "==> Configuring (pico_w board)"
rm -rf build
mkdir build
cmake -S . -B build \
    -DPICO_SDK_PATH="$PICO_SDK_PATH" \
    -DPIMORONI_PICO_PATH="$PIMORONI_PICO_PATH" \
    -DPICO_BOARD=pico_w

echo "==> Building"
cmake --build build -j"$(nproc)"

echo ""
echo "Build complete. Flash build/hello_world.uf2 to your Pico W:"
echo "  Hold BOOTSEL, plug in USB, then: cp build/hello_world.uf2 /media/\$USER/RPI-RP2/"
