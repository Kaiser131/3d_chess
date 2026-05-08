cmake_minimum_required(VERSION 3.20)
project(3d_chess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

add_executable(3d_chess
  src/main.cpp
  src/Camera.cpp
  src/Input.cpp
  src/Board.cpp
  src/Piece.cpp
  src/Renderer.cpp
  src/ChessGame.cpp
)

target_include_directories(3d_chess PRIVATE src)

target_compile_definitions(3d_chess PRIVATE _USE_MATH_DEFINES)

if (MSVC)
  target_compile_options(3d_chess PRIVATE /W4 /permissive-)
else()
  target_compile_options(3d_chess PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(3d_chess PRIVATE glfw OpenGL::GL)

# GLU: some CMake setups export OpenGL::GLU, some don't (Windows often needs glu32)
if (TARGET OpenGL::GLU)
  target_link_libraries(3d_chess PRIVATE OpenGL::GLU)
elseif (WIN32)
  target_link_libraries(3d_chess PRIVATE glu32)
endif()

# On Windows, keep the console for logs/debugging
set_target_properties(3d_chess PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
