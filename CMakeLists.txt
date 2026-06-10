cmake_minimum_required(VERSION 3.16)
project(Biquadris CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TEXT_ONLY "Build without graphics (no SDL2 required)" OFF)

# ─── Source files ─────────────────────────────────────────────────────────────
set(SOURCES
    main.cc
    game.cc
    board.cc
    block.cc
    level.cc
    effect.cc
    textdisplay.cc
)

if(NOT TEXT_ONLY)
    list(APPEND SOURCES window.cc)
endif()

add_executable(biquadris ${SOURCES})
target_compile_options(biquadris PRIVATE -Wall)

# ─── Graphics (SDL2 + SDL2_ttf) ───────────────────────────────────────────────
if(NOT TEXT_ONLY)
    # On macOS with Homebrew, SDL2 lives outside the default search paths.
    # Ask brew for the prefix so find_package can locate the cmake configs.
    if(APPLE)
        execute_process(COMMAND brew --prefix sdl2
            OUTPUT_VARIABLE _SDL2_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
        execute_process(COMMAND brew --prefix sdl2_ttf
            OUTPUT_VARIABLE _SDL2_TTF_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
        if(_SDL2_PREFIX)
            list(APPEND CMAKE_PREFIX_PATH "${_SDL2_PREFIX}")
        endif()
        if(_SDL2_TTF_PREFIX)
            list(APPEND CMAKE_PREFIX_PATH "${_SDL2_TTF_PREFIX}")
        endif()
    endif()

    find_package(SDL2     REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    target_compile_definitions(biquadris PRIVATE BIQUADRIS_GRAPHICS)
    # Explicitly add include dirs — some cmake configs set INTERFACE_INCLUDE_DIRECTORIES
    # to the SDL2 subdirectory, so we also need the parent for <SDL.h> resolution.
    target_include_directories(biquadris PRIVATE
        ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
    # SDL2main provides WinMain on Windows; harmless no-op on macOS/Linux
    target_link_libraries(biquadris PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_ttf::SDL2_ttf)
endif()

# ─── Copy data files next to the binary so the game finds them at runtime ─────
configure_file(biquadris_sequence1.txt biquadris_sequence1.txt COPYONLY)
configure_file(biquadris_sequence2.txt biquadris_sequence2.txt COPYONLY)
