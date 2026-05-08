# Code::Blocks setup (easy demo build)

This repository includes a **freeglut/GLUT entrypoint** so you can build and run directly in Code::Blocks without CMake or GLFW.

## 1) Install freeglut (MinGW)

You need these libraries available to your compiler/linker:
- `opengl32`
- `glu32`
- `glut32` **or** `freeglut` (depends on your GLUT/freeglut package)

Common options:
- **MSYS2**: install `mingw-w64-x86_64-freeglut` and use the MSYS2 MinGW toolchain in Code::Blocks.
- Or download a **freeglut MinGW** package and point Code::Blocks to its `include/` and `lib/` folders.

## 2) Open the project

Open the Code::Blocks project file:
- `codeblocks/3d_chess_glut.cbp`

This `.cbp` is configured to link **`glut32`** by default (commonly bundled with Code::Blocks/MinGW).
If your installation provides **`freeglut`** instead, change the linker library in Code::Blocks:
- Project → Build options → Linker settings → replace `glut32` with `freeglut`

## 3) Configure search paths (one-time)

In Code::Blocks:
- Settings → Compiler → Search directories
  - **Compiler** tab: add your freeglut `include` path (contains `GL/freeglut.h`)
  - **Linker** tab: add your freeglut `lib` path (contains `libfreeglut.a` / `freeglut.lib`)

Tip: this project includes `#include <GL/glut.h>` on Windows. So your include folder must contain `GL/glut.h` (most freeglut installs provide it).

## 4) Build & run

Build/Run the target.

## Controls
- Left click: select/move
- Right (or middle) mouse drag: orbit camera
- Mouse wheel: zoom (also works via wheel up/down as buttons 3/4 on freeglut)
- Arrow keys: orbit camera
- +/- : zoom
- `R`: reset camera
- `Esc`: quit

## Notes
- Movement rules + captures + turns are implemented.
- No check/checkmate, castling, or en-passant.
