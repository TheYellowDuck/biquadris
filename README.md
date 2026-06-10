# Biquadris

A two-player competitive Tetris variant built in C++. Originally developed as a final project for **CS 246: Object-Oriented Software Development** at the **University of Waterloo** (Fall 2025) by a team of three students.

The game has since been refactored and extended beyond the original submission: bugs fixed, the X11 graphics layer replaced with SDL2 for cross-platform support, and the build system migrated to CMake for easy packaging.

---

## What is Biquadris?

Biquadris is a turn-based, two-player spin on Tetris. Both players share one screen — each managing their own 11×18 board. Players alternate turns: you make one move, then your opponent does. Clear two or more lines in a single drop and you earn a **special action** to punish your opponent. The game ends when a player can no longer place a new block.

---

## Building

### Requirements

- C++20 compiler (g++ 14+ or clang++ 17+)
- CMake 3.16+
- SDL2 (`brew install sdl2` on macOS, `sudo apt install libsdl2-dev` on Linux)

### Build

```bash
cmake -B build
cmake --build build
```

The binary is placed at `build/biquadris`.

Text-only mode (no SDL2 required):

```bash
cmake -B build -DTEXT_ONLY=ON
cmake --build build
```

---

## Running

```bash
./build/biquadris                          # graphical mode (default)
./build/biquadris -text                   # text-only mode
./build/biquadris -seed 42 -startlevel 2  # custom seed and starting level
```

### Running from a release download (macOS)

macOS quarantines all files downloaded from the internet. Remove the quarantine attribute from the entire folder before running:

```bash
xattr -dr com.apple.quarantine .
./biquadris
```

### Command-line options

| Flag | Default | Description |
| ---- | ------- | ----------- |
| `-text` | off | Run in text-only mode |
| `-seed N` | `0` | Random seed for block generation |
| `-scriptfile1 FILE` | `biquadris_sequence1.txt` | Block sequence file for Player 1 (Level 0) |
| `-scriptfile2 FILE` | `biquadris_sequence2.txt` | Block sequence file for Player 2 (Level 0) |
| `-startlevel N` | `0` | Starting difficulty level (0–4) |

---

## Controls

### Graphical mode

| Key | Action |
| --- | ------ |
| Arrow Left / Right | Move block |
| Arrow Down | Move block down one row |
| Arrow Up / X | Rotate clockwise |
| Z | Rotate counter-clockwise |
| Space | Hard drop |
| + / = | Level up |
| - | Level down |
| R | Restart |
| Q | Quit |
| 1–9 (then action) | Multiplier — e.g. `3` → Left Arrow = move left 3 |

### Text mode commands

Commands support unique prefix matching (e.g. `le` = `left`, `cl` = `clockwise`) and numeric multipliers (e.g. `3right`, `2drop`).

| Command | Description |
| ------- | ----------- |
| `left`, `right`, `down` | Move block |
| `clockwise`, `counterclockwise` | Rotate block |
| `drop` | Hard drop |
| `levelup`, `leveldown` | Change difficulty |
| `norandom <file>` | Use a sequence file for block generation (levels 3–4) |
| `random` | Return to random block generation |
| `restart` | Start a new game |
| `sequence <file>` | Read and execute commands from a file |
| `I J L O S T Z` | Replace current block with that type (for testing) |

---

## Levels

| Level | Description |
| ----- | ----------- |
| 0 | Blocks drawn from the sequence files in order (non-random) |
| 1 | Random; S and Z pieces appear half as often |
| 2 | All 7 pieces equally likely |
| 3 | S and Z more likely; every block is **heavy** (drops 1 extra row per move) |
| 4 | Level 3 rules + a `*` penalty block is added to the center column every 5 blocks placed without a line clear |

---

## Special Actions

Clearing **2 or more lines** in a single drop earns a special action against your opponent:

| Action | Effect |
| ------ | ------ |
| `blind` | Covers columns 3–9, rows 3–14 of the opponent's board with `?` until their next drop |
| `heavy` | Opponent's blocks drop 2 extra rows after every move/rotation for one turn |
| `force <type>` | Replaces the opponent's **current** block with the specified type (I/J/L/O/S/T/Z) |

---

## Scoring

- **Line clear**: `(current_level + lines_cleared)²` points
- A block that scores zero lines resets the combo; clearing any lines resets the no-clear counter (relevant for Level 4 penalty blocks)
- High score is saved across sessions in `.biquadris_highscore`

---

## Project Structure

```text
biquadris/
├── main.cc           — Entry point and argument parsing
├── game.cc/h         — Game loop, command parsing, player switching
├── board.cc/h        — Board state, block placement, line clearing
├── block.cc/h        — Tetromino shapes and rotation
├── level.cc/h        — Block generation per difficulty level
├── effect.cc/h       — Special action effect system (blind, heavy, force)
├── textdisplay.cc/h  — Terminal renderer
├── window.cc/h       — SDL2 graphical renderer
├── CMakeLists.txt    — Cross-platform build
└── biquadris_sequence{1,2}.txt — Default Level 0 block sequences
```

---

## Origins

This project was originally written for **CS 246** at UW by a group of three students. The course project required implementing a working two-player Tetris game with text display, multiple difficulty levels, and the special action system documented above.

Post-submission changes:

- Fixed a block-skipping bug in multiplier drops (`checkNextBlock` called twice)
- Removed duplicate block-color mapping code (extracted `blockColor` helper)
- Fixed `force` to replace the opponent's **current** block (was incorrectly replacing the next queued block)
- Replaced X11/XQuartz graphics with SDL2 for cross-platform compatibility
- Migrated build system from `Makefile` to CMake for packaging support
- Overhauled SDL2 UI: dark theme, ghost block preview, bevel-shaded blocks, active-effects display
- Cleared all compiler warnings
