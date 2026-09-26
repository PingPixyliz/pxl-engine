# Third-party dependencies, fetched at configure time and pinned by URL + SHA-256.
# To upgrade one: change its URL, then copy the new hash from CMake's hash-mismatch error.
#
# GLM and stb are header-only: SOURCE_SUBDIR names a directory that does not exist, so
# FetchContent downloads the archive without running the library's own CMakeLists.txt.
# Include directories are SYSTEM so the project's warning flags skip third-party headers.

include_guard(GLOBAL)

include(FetchContent)

FetchContent_Declare(glm
    URL      "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz"
    URL_HASH "SHA256=9f3174561fd26904b23f0db5e560971cbf9b3cbda0b280f04d5c379d03bf234c"
    SOURCE_SUBDIR "_headers_only_"
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(glm)

add_library(pxl_glm INTERFACE)
add_library(pxl::glm ALIAS pxl_glm)

target_include_directories(pxl_glm SYSTEM INTERFACE "${glm_SOURCE_DIR}")
target_compile_definitions(pxl_glm INTERFACE
    # WebGPU clip space depth is [0, 1] (OpenGL's is [-1, 1]).
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

# stb has no release tags, so a commit is pinned instead (2026-08-02).
FetchContent_Declare(stb
    URL      "https://github.com/nothings/stb/archive/2c980bb59875b0d32144a71867fbdebb2f77cd20.tar.gz"
    URL_HASH "SHA256=9a955b1b49a4410088a2e0ee2a9c057c3c907d0c1d75454144cb980aca0ba515"
    SOURCE_SUBDIR "_headers_only_"
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(stb)

add_library(pxl_stb INTERFACE)
add_library(pxl::stb ALIAS pxl_stb)

target_include_directories(pxl_stb SYSTEM INTERFACE "${stb_SOURCE_DIR}")

# fmt 12 turns on C++20 module scanning, which fails with Emscripten, so both are switched off.
FetchContent_Declare(fmt
    URL      "https://github.com/fmtlib/fmt/releases/download/12.2.0/fmt-12.2.0.zip"
    URL_HASH "SHA256=a2f4a8d51178f954e4c339007f77edd76ba0cb2e36f87a48e5a5403d9be5878f"
    SYSTEM
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
block()
    set(FMT_MODULE OFF)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
    FetchContent_MakeAvailable(fmt)
endblock()

# No locale support and less inlining: the release wasm is about half the size of the default.
target_compile_definitions(fmt PUBLIC FMT_OPTIMIZE_SIZE=2)
