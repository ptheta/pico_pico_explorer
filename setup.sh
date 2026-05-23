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

# --- picotool ---
# The pico-sdk embeds picotool but passes -DPICOTOOL_NO_LIBUSB=1 and its embedded
# build breaks on GCC 16. Build it separately with -fpermissive and install it so
# that the pico-sdk finds the pre-built version instead of rebuilding it.
if [ ! -d "$PICO_DIR/picotool" ]; then
    echo "==> Cloning picotool"
    git clone --depth 1 https://github.com/raspberrypi/picotool.git "$PICO_DIR/picotool"
else
    echo "==> picotool already present"
fi

echo "==> Building picotool"
cmake -S "$PICO_DIR/picotool" -B "$PICO_DIR/picotool/build" \
    -DPICO_SDK_PATH="$PICO_SDK_PATH" \
    -DPICOTOOL_NO_LIBUSB=1 \
    -DPICOTOOL_FLAT_INSTALL=1 \
    -DCMAKE_INSTALL_PREFIX="$PICO_DIR/picotool-install" \
    -DCMAKE_CXX_FLAGS="-fpermissive -Wno-error"
cmake --build "$PICO_DIR/picotool/build" -j"$(nproc)"
cmake --install "$PICO_DIR/picotool/build"

# --- build ---
echo "==> Configuring (pico_w board)"
rm -rf build
cmake -S . -B build \
    -DPICO_SDK_PATH="$PICO_SDK_PATH" \
    -DPIMORONI_PICO_PATH="$PIMORONI_PICO_PATH" \
    -DPICO_BOARD=pico_w \
    -DCMAKE_PREFIX_PATH="$PICO_DIR/picotool-install"

echo "==> Building"
cmake --build build -j"$(nproc)"

echo ""
echo "Build complete. Flash build/hello_world.uf2 to your Pico W:"
echo "  Hold BOOTSEL, plug in USB, then run: bash flash.sh"
