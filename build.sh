
# ═══════════════════════════════════════════════════════
#  Build Script for stone Scientific Calculator
# ═══════════════════════════════════════════════════════

echo "Building Stone Scientific Calculator..."

g++ -o ScientificCalculator.exe ScientificCalculator.cpp \
    -lgdi32 -luser32 -lcomctl32 \
    -mwindows \
    -std=c++17 \
    -O2 \
    -Wall \
    -static -static-libgcc -static-libstdc++

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Build successful! Run: ./ScientificCalculator.exe"
else
    echo ""
    echo "✗ Build failed. Make sure you're in MSYS2 MinGW64 shell."
fi
