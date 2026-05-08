# qr-generator

A QR code generator written in C from scratch, featuring GF(256) arithmetic and Reed-Solomon error correction, with SDL3 rendering.

## Features

- **GF(256) arithmetic** — addition, multiplication, division, and inversion in Galois Field 2⁸ using the QR standard irreducible polynomial (0x11D)
- **Polynomial math over GF(256)** — add, multiply, divide, and scale polynomials for use in Reed-Solomon encoding
- **Reed-Solomon ECC** — full error correction codeword generation with multi-block interleaving for all QR versions
- **QR code generation pipeline** — complete ISO/IEC 18004 compliant pipeline including:
  - Numeric, alphanumeric, and byte encoding modes with auto-detection
  - All four error correction levels: L, M, Q, H
  - Finder, timing, alignment, and dark module placement
  - Format information strings (all ECL/mask combinations)
  - Version information strings (versions 7–40)
  - Zigzag data placement with visited cell tracking
  - Automatic mask pattern selection via penalty scoring
- **SDL3 rendering** — renders the generated QR matrix to a fixed-size window with quiet zone padding

## Current Limitations

- Kanji encoding mode is not yet implemented
- Mask penalty scoring sometimes picks suboptimal mask (still works despite that)
- No QR input for decoding

## Dependencies

- [SDL3](https://github.com/libsdl-org/SDL)
- [CMake](https://cmake.org/) 3.16+

### Installing SDL3

**macOS (Homebrew):**
```bash
brew install sdl3
```

**Linux (build from source):**
```bash
git clone https://github.com/libsdl-org/SDL.git
cd SDL
cmake -B build
cmake --build build
sudo cmake --install build
```

**Windows:**
Download the **SDL3 development libraries** from the [SDL3 releases page](https://github.com/libsdl-org/SDL/releases):
- `SDL3-devel-x.x.x-VC.zip` if using Visual Studio (MSVC)
- `SDL3-devel-x.x.x-mingw.zip` if using MinGW/GCC
```bash
cmake -B build -DSDL3_DIR="C:/path/to/SDL3/cmake"
cmake --build build
```

## Building

```bash
cmake -B build
cmake --build build
```

The executable will be at `build/qrgen` on macOS/Linux or `build/Debug/qrgen.exe` on Windows.

## Usage

```bash
./build/qrgen -M <encoding-type> -L <ecl> -D <data>
./build/qrgen -M <encoding-type> -L <ecl> -- "<data>"
```

### Arguments

| Flag | Values | Description |
|------|--------|-------------|
| `-M` | `numeric` \| `alphanumeric` \| `byte` \| `auto` | Encoding mode (`auto` detects the most compact mode) |
| `-L` | `L` \| `M` \| `Q` \| `H` | Error correction level |
| `-D` | any string | Data to encode |
| `--` | any string | Data to encode (use when data starts with `-`) |

### Examples

```bash
# Auto-detect encoding mode
./build/qrgen -M auto -L M -D "Hello, World!"

# Force byte mode with high error correction
./build/qrgen -M byte -L H -D "https://github.com"

# Numeric mode
./build/qrgen -M numeric -L L -D "1234567890"

# Alphanumeric mode
./build/qrgen -M alphanumeric -L M -D "HELLO WORLD"

# Data starting with a dash
./build/qrgen -M byte -L M -- "-my-data"
```

## Project Structure

```
.
├── include
│   ├── ANSI-color-codes.h  - third-party ANSI color macros
│   ├── galois_field.h      - GF(256) field operations
│   ├── gui.h               - SDL3 window and rendering
│   ├── input_handler.h     - CLI argument parsing
│   ├── logger.h            - error logging
│   ├── poly_math.h         - GF(256) polynomial arithmetic
│   ├── qr_capacity.h       - version/ECL capacity and block tables
│   ├── qr_format.h         - format strings, version strings, alignment positions
│   ├── qr_generator.h      - QR generation pipeline
│   └── rs_encoding.h       - Reed-Solomon ECC
├── src
│   ├── galois_field.c      - GF(256) arithmetic implementation
│   ├── gui.c               - SDL3 rendering implementation
│   ├── input_handler.c     - argument parsing implementation
│   ├── logger.c            - colored error messages
│   ├── main.c              - entry point and event loop
│   ├── poly_math.c         - polynomial arithmetic implementation
│   ├── qr_generator.c      - full QR generation pipeline
│   └── rs_encoding.c       - Reed-Solomon encoding
├── docs                    - Doxygen generated documentation
├── CMakeLists.txt
├── Makefile
└── README.md
```

## Documentation

Documentation is generated with Doxygen. To regenerate:

```bash
doxygen Doxyfile
open docs/html/index.html
```

## Third-party

- [ANSI-color-codes.h](https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a) — Public domain ANSI terminal color macros (Unlicense)

## License

MIT