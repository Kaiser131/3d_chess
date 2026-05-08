# 3D Chess (OpenGL + GLFW)

Interactive 3D chess demo with basic chess rules, mouse picking, camera orbit controls, and fixed-function OpenGL lighting.

## Easy run in Code::Blocks (recommended for demos)

This repo also includes a **freeglut/GLUT build** specifically to make Code::Blocks usage simple:
- Open `codeblocks/3d_chess_glut.cbp`
- Follow the short guide in `CODEBLOCKS_SETUP.md`

## Build (Windows)

Prereqs:
- Visual Studio 2022 (or Build Tools)
- CMake 3.20+

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Run:
```bash
./build/Release/3d_chess.exe
```

## Controls

- **Left click**: select piece / click destination to move
- **Right mouse drag**: rotate camera (orbit)
- **Mouse wheel**: zoom in/out
- **R**: reset camera
- **Esc**: quit

## Notes

- Implements movement rules for all pieces, captures, turns, basic pawn promotion (auto-queen).
- Does **not** implement check/checkmate/castling/en-passant.
