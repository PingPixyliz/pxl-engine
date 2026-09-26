# pxl::webgpu provides <webgpu/webgpu.h> and <webgpu/webgpu_cpp.h> from Dawn's emdawnwebgpu port.
# --use-port is needed at compile time (headers) and at link time (JS glue), so it lives on an
# INTERFACE library that passes it to everything linking it.
# Never add -sUSE_WEBGPU: it selects Emscripten's removed legacy bindings and conflicts with the port.

include_guard(GLOBAL)

add_library(pxl_webgpu INTERFACE)
add_library(pxl::webgpu ALIAS pxl_webgpu)

target_compile_options(pxl_webgpu INTERFACE "--use-port=${PXL_EMDAWNWEBGPU_PORT}")
target_link_options(pxl_webgpu INTERFACE "--use-port=${PXL_EMDAWNWEBGPU_PORT}")
