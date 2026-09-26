cmake_minimum_required(VERSION 3.25)

project(pxl
    VERSION 0.1.0
    DESCRIPTION "C++20 WebGPU engine for the web (WebAssembly via Emscripten)"
    LANGUAGES CXX
)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR
        "This project targets the web and must be configured with the Emscripten toolchain.\n"
        "Use one of the presets (they pick up the toolchain from the EMSDK environment variable):\n"
        "    cmake --preset web-debug\n"
        "or simply run:\n"
        "    python scripts/build.py"
    )
endif()

# The emdawnwebgpu "remote port" mechanism requires Emscripten 4.0.10 or newer.
set(PXL_MIN_EMSCRIPTEN_VERSION "4.0.10")
if(EMSCRIPTEN_VERSION VERSION_LESS PXL_MIN_EMSCRIPTEN_VERSION)
    message(FATAL_ERROR
        "Emscripten ${EMSCRIPTEN_VERSION} is too old; "
        "${PXL_MIN_EMSCRIPTEN_VERSION} or newer is required (emsdk install latest)."
    )
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use a preset or pass -B <build-dir>.")
endif()

option(PXL_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(PXL_ENABLE_CLOSURE "Minify the generated JavaScript with the Closure compiler (Release only)" OFF)

set(PXL_EMDAWNWEBGPU_PORT "emdawnwebgpu" CACHE STRING
    "--use-port value for the WebGPU bindings: the bundled port, or a path to a newer emdawnwebgpu-*.remoteport.py")

set(PXL_DIST_DIR "${CMAKE_BINARY_DIR}/dist" CACHE PATH
    "Directory that receives the deployable website (one sub-folder per application)")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(PxlCompilerWarnings)
include(PxlWebGpu)
include(PxlDependencies)
include(PxlEmbedShaders)
include(PxlWebApp)

add_subdirectory(engine)
add_subdirectory(samples)
