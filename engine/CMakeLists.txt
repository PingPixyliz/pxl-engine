file(GLOB_RECURSE engine_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pxl/*.cpp")

add_library(pxl_engine STATIC ${engine_sources})
add_library(pxl::engine ALIAS pxl_engine)
pxl_set_warnings(pxl_engine)

target_include_directories(pxl_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(pxl_engine PUBLIC cxx_std_20)
target_link_libraries(pxl_engine
    PUBLIC pxl::webgpu pxl::glm fmt::fmt
    PRIVATE pxl::stb
)
target_link_options(pxl_engine PUBLIC "-sFETCH=1")
