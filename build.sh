#!/usr/bin/env bash
# Build lashim.dll (x86_64) with mingw-w64.
#
# You do NOT need to build anything to use this - a prebuilt lashim.dll ships
# alongside this script. Build only if you have changed lashim.cpp.
#
# SteamOS is immutable and has no compiler. Build inside a container:
#
#   distrobox create --name build --image archlinux:latest
#   distrobox enter build
#   sudo pacman -Syu --noconfirm mingw-w64-gcc
#   cd /path/to/lashim && ./build.sh
#
# or on Debian/Ubuntu:  sudo apt install g++-mingw-w64-x86-64
# or on Fedora:         sudo dnf install mingw64-gcc-c++

set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-}"
if [[ -z "$CXX" ]]; then
    for c in x86_64-w64-mingw32-g++ x86_64-w64-mingw32-g++-win32 x86_64-w64-mingw32-c++; do
        if command -v "$c" >/dev/null 2>&1; then CXX="$c"; break; fi
    done
fi

if [[ -z "$CXX" ]]; then
    echo "error: no x86_64-w64-mingw32-g++ found." >&2
    echo "       install mingw-w64 (see the header of this script) or set CXX=." >&2
    exit 1
fi

echo "using $CXX"

"$CXX" \
    -O2 -Wall -Wextra -Wno-unused-parameter -Wno-delete-non-virtual-dtor \
    -fno-exceptions -fno-rtti \
    -shared -o lashim.dll \
    lashim.cpp lashim.def \
    -static -static-libgcc -static-libstdc++ \
    -Wl,--enable-stdcall-fixup \
    -lole32 -loleaut32 -luuid -ladvapi32 -lgdi32 -luser32

echo
echo "built: $(pwd)/lashim.dll"
echo "md5:   $(md5sum lashim.dll | cut -d' ' -f1)"
echo
echo "expected exports: DllCanUnloadNow DllGetClassObject DllRegisterServer DllUnregisterServer"
x86_64-w64-mingw32-objdump -p lashim.dll 2>/dev/null \
    | sed -n '/\[Ordinal\/Name Pointer\] Table/,/^$/p' || true
echo "imports (should be KERNEL32 / ADVAPI32 / USER32 / msvcrt only):"
x86_64-w64-mingw32-objdump -p lashim.dll 2>/dev/null | grep 'DLL Name' || true
