# qr-generator

A QR code generator written in C using SDL3 for rendering.

## Features

- Generates QR codes from command line input
- Reed-Solomon error correction
- Renders QR code to a window using SDL3
- Quiet zone padding around the code

## Current Status

Working:
- Byte mode encoding
- ECL L error correction
- Reed-Solomon ECC via GF(256) polynomial math
- Function patterns (finder patterns, timing patterns, dark module)
- Data placement via zigzag scan
- Mask pattern application
- Format string placement
- SDL3 rendering

In progress / not yet supported:
- Numeric, alphanumeric, and Kanji encoding modes
- Automatic mask scoring and selection (currently hardcoded)
- Versions higher than 1 (alignment patterns, version info strings)
- Multi-block interleaving for higher versions

## Dependencies

- [SDL3](https://github.com/libsdl-org/SDL)

## Building

```bash
mkdir build
make
```

## Usage

```bash
./build/qrgen -D "Hello, world! 123" -L L -M byte
```

## Project Structure
```
.
├── build
│   └── qrgen               - final executable
├── include
│   ├── ANSI-color-codes.h
│   ├── galois_field.h
│   ├── gui.h
│   ├── input_handler.h
│   ├── logger.h
│   ├── poly_math.h
│   ├── qr_capacity.h       - version/ECL capacity and block tables
│   ├── qr_format.h         - format and version information strings
│   ├── qr_generator.h
│   └── rs_encoding.h
├── LICENSE
├── Makefile
├── README.md
├── src
│   ├── galois_field.c      - GF(256) field operations
│   ├── gui.c               - SDL3 rendering
│   ├── input_handler.c     - CLI argument parsing
│   ├── logger.c            - error logging
│   ├── main.c              - entry point and event loop
│   ├── poly_math.c         - GF(256) polynomial arithmetic
│   ├── qr_generator.c      - QR code generation pipeline
│   └── rs_encoding.c       - Reed-Solomon ECC
└── version.txt
```

## Third-party

- [ANSI-color-codes.h](https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a) — Public domain ANSI terminal color macros (Unlicense)

## License

MIT