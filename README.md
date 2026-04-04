# Arcade

A retro gaming platform built in C++ using dynamic libraries loaded at runtime.

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
cd ..
```

This generates the `arcade` binary and all `.so` libraries in `./lib/`.

## Usage

```bash
./arcade ./lib/arcade_ncurses.so
```

Pass any graphical library as the startup argument. You can switch libraries at runtime using the keybindings below.

## Keybindings

| Key | Action |
|-----|--------|
| `F1` | Previous graphical library |
| `F2` | Next graphical library |
| `F3` | Previous game |
| `F4` | Next game |
| `R` | Restart current game |
| `M` | Go back to menu |
| `Escape` | Exit |

## Graphical libraries

| Library | File |
|---------|------|
| nCurses | `lib/arcade_ncurses.so` |
| SDL2 | `lib/arcade_sdl2.so` |
| SFML | `lib/arcade_sfml.so` |

## Games

| Game | File |
|------|------|
| Snake | `lib/arcade_snake.so` |
| Minesweeper | `lib/arcade_minesweeper.so` |

## Project structure

```
.
├── CMakeLists.txt
├── src/
│   └── core/
├── lib/
│   ├── arcade_ncurses.so
│   ├── arcade_sdl2.so
│   ├── arcade_sfml.so
│   └── ...
├── doc/
│   └── ...
└── README.md
```

## Authors

- jean-baptiste.lamy-floret@epitech.eu

