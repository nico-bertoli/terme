#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR" || exit

BUILD_TYPE="Release"

# ====================================================== create build folder
mkdir -p build
cd build || exit

# ====================================================== helper (aligned with terme_examples/xbuild.sh)
create_nbkit_from_github() {
    echo "[INFO] Creating Conan package nbkit/1.0.0 from GitHub..."
    TMP_DIR=$(mktemp -d)
    git clone --depth 1 https://github.com/nico-bertoli/nbkit.git "$TMP_DIR/nbkit"
    # No --user/--channel: must match requires("nbkit/1.0.0") in conanfile.py
    conan create "$TMP_DIR/nbkit" --name=nbkit --version=1.0.0 \
        -s "build_type=$BUILD_TYPE" -s compiler.cppstd=20 --build=missing || {
        rm -rf "$TMP_DIR"
        exit 1
    }
    rm -rf "$TMP_DIR"
}

if ! conan list "nbkit/1.0.0#*" 2>/dev/null | grep -q "revisions"; then
    create_nbkit_from_github
else
    echo "[INFO] Found nbkit/1.0.0 in local cache."
fi

# ====================================================== x11 dev packages error message
conan install .. --build=missing \
    -s "build_type=$BUILD_TYPE" \
    -s compiler.cppstd=20 \
    -c tools.system.package_manager:mode=skip || {
    echo ""
    echo "=========================================="
    echo "ERROR: the xorg/system package requires X11 development packages"
    echo "=========================================="
    echo ""
    echo "Please run: sudo apt-get install libx11-xcb-dev libxcb-render0-dev libxcb-render-util0-dev libxcb-xkb-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-dri3-dev uuid-dev libxcb-cursor-dev libxcb-dri2-0-dev libxcb-present-dev libxcb-composite0-dev libxcb-ewmh-dev libxcb-res0-dev libfontenc-dev libice-dev libsm-dev libxaw7-dev libxcomposite-dev libxcursor-dev libxdamage-dev libxext-dev libxfixes-dev libxi-dev libxinerama-dev libxkbfile-dev libxmu-dev libxmuu-dev libxpm-dev libxrandr-dev libxrender-dev libxres-dev libxss-dev libxt-dev libxtst-dev libxv-dev libxxf86vm-dev libxcb-glx0-dev"
    echo ""
    exit 1
}

# ====================================================== find the conan toolchain file
CMAKE_TOOLCHAIN=$(find . -name conan_toolchain.cmake | head -n 1)

if [ -z "$CMAKE_TOOLCHAIN" ]; then
    echo "ERROR: Could not find conan_toolchain.cmake"
    exit 1
fi

# ====================================================== build
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="$CMAKE_TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_EXE_LINKER_FLAGS="-lX11 -lpthread -ldl" \
    -DCMAKE_SHARED_LINKER_FLAGS="-lX11"

cmake --build . --config "$BUILD_TYPE" -- -j"$(nproc)"
