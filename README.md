# Stone Scientific Calculator
### A scientific calculator built with C++ & Win32 API

---

## Features

- **Basic Arithmetic** — +, −, ×, ÷, %, parentheses
- **Scientific Functions** — sin, cos, tan, asin, acos, atan
- **Logarithms** — log₁₀, ln, 10^x, e^x
- **Power & Roots** — x², x³, xʸ, √x, ∛x, ʸ√x
- **Special** — 1/x, |x|, n!, π, e constant, EE (scientific notation)
- **Memory** — MC, MR, M+, M−
- **Angle Modes** — DEG / RAD toggle
- **2nd Functions** — Press [2nd] to access alternate functions on each key
- **Keyboard Support** — Type numbers and operators directly
- **Smooth UI** — Dark theme, double-buffered rendering, hover effects

---

## Building with MSYS2

### Step 1 – Install MSYS2
Download from: https://www.msys2.org/

### Step 2 – Open MinGW64 terminal
Launch: **MSYS2 MinGW 64-bit** (from Start Menu)

### Step 3 – Install GCC (first time only)
```bash
pacman -S mingw-w64-x86_64-gcc
```

### Step 4 – Build
Navigate to this folder and run:
```bash
bash build.sh
```
Or manually:
```bash
g++ -o ScientificCalculator.exe ScientificCalculator.cpp \
    -lgdi32 -luser32 -lcomctl32 \
    -mwindows -std=c++17 -O2 \
    -static -static-libgcc -static-libstdc++
```

### Step 5 – Run
```bash
./ScientificCalculator.exe
```
Or double-click `ScientificCalculator.exe` in Windows Explorer.

---

## Keyboard Shortcuts

| Key          | Action              |
|-------------|---------------------|
| 0–9, .      | Input digits/decimal|
| + - * /     | Operators           |
| ^ %         | Power, Modulo       |
| ( )         | Parentheses         |
| Enter       | Evaluate (=)        |
| Backspace   | Delete last char    |
| Escape      | Clear (AC)          |

---

## Notes
- No external dependencies — pure Win32 API
- Single `.cpp` file, single `.exe` output
- Works on Windows 7 / 8 / 10 / 11
