# pxl_add_web_app(<target>
#     TITLE        <text>
#     SOURCES      <file>...
#     [SHADERS     <file.wgsl>...]   embedded with pxl_embed_shaders()
#     [ASSETS      <dir>]            copied to <PXL_DIST_DIR>/<target>/<dir name>/, fetched as "<dir name>/<file>"
#     [LIBRARIES   <target>...]      linked in addition to pxl::engine
# )
#   Builds <PXL_DIST_DIR>/<target>/index.{html,js,wasm}, served at <site>/<target>/.
#
# pxl_set_default_app(<target>)
#   Writes <PXL_DIST_DIR>/index.html, which redirects the site root to <target>/.

include_guard(GLOBAL)

set(PXL_WEB_DIR "${PROJECT_SOURCE_DIR}/web")

# One command per file: editing an asset recopies only that file, without relinking.
function(_pxl_copy_assets target dir)
    cmake_path(ABSOLUTE_PATH dir BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE source_dir)
    cmake_path(GET source_dir FILENAME dir_name)
    file(GLOB_RECURSE assets CONFIGURE_DEPENDS "${source_dir}/*")

    set(copied "")
    foreach(asset IN LISTS assets)
        cmake_path(RELATIVE_PATH asset BASE_DIRECTORY "${source_dir}" OUTPUT_VARIABLE relative)
        set(output "${PXL_DIST_DIR}/${target}/${dir_name}/${relative}")
        cmake_path(GET output PARENT_PATH output_dir)
        add_custom_command(
            OUTPUT "${output}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${output_dir}"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${asset}" "${output}"
            DEPENDS "${asset}"
            COMMENT "Copying asset ${relative}"
            VERBATIM
        )
        list(APPEND copied "${output}")
    endforeach()

    add_custom_target(${target}_assets DEPENDS ${copied})
    add_dependencies(${target} ${target}_assets)
endfunction()

function(pxl_add_web_app target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "TITLE;ASSETS" "SOURCES;SHADERS;LIBRARIES")

    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "pxl_add_web_app(${target}): unknown arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT arg_SOURCES)
        message(FATAL_ERROR "pxl_add_web_app(${target}): SOURCES is required")
    endif()
    if(NOT arg_TITLE)
        set(arg_TITLE "${target}")
    endif()

    add_executable(${target} ${arg_SOURCES})
    target_link_libraries(${target} PRIVATE pxl::engine ${arg_LIBRARIES})
    pxl_set_warnings(${target})

    if(arg_SHADERS)
        pxl_embed_shaders(${target} ${arg_SHADERS})
    endif()

    if(arg_ASSETS)
        _pxl_copy_assets(${target} "${arg_ASSETS}")
    endif()

    set(shell_file "${CMAKE_CURRENT_BINARY_DIR}/${target}.shell.html")
    set(PXL_APP_TITLE "${arg_TITLE}")
    configure_file("${PXL_WEB_DIR}/shell.html.in" "${shell_file}" @ONLY)

    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "index"
        SUFFIX ".html"
        RUNTIME_OUTPUT_DIRECTORY "${PXL_DIST_DIR}/${target}"
        LINK_DEPENDS "${shell_file}"
        PXL_APP_TITLE "${arg_TITLE}"
    )

    target_link_options(${target} PRIVATE
        "--shell-file=${shell_file}"
        "-sENVIRONMENT=web"
        "-sALLOW_MEMORY_GROWTH=1"
        "$<$<CONFIG:Debug>:-sASSERTIONS=1>"
        "$<$<CONFIG:Debug>:-sSTACK_OVERFLOW_CHECK=2>"
        "$<$<AND:$<CONFIG:Release>,$<BOOL:${PXL_ENABLE_CLOSURE}>>:--closure=1>"
    )

    set_property(GLOBAL APPEND PROPERTY PXL_WEB_APPS ${target})
endfunction()

function(pxl_set_default_app target)
    get_property(apps GLOBAL PROPERTY PXL_WEB_APPS)
    if(NOT target IN_LIST apps)
        message(FATAL_ERROR "pxl_set_default_app(${target}): no app of that name; "
            "call it after pxl_add_web_app(${target})")
    endif()

    get_property(existing GLOBAL PROPERTY PXL_DEFAULT_APP)
    if(existing)
        message(FATAL_ERROR "pxl_set_default_app(${target}): the default app is already '${existing}'")
    endif()
    set_property(GLOBAL PROPERTY PXL_DEFAULT_APP ${target})

    set(PXL_DEFAULT_APP "${target}")
    get_target_property(PXL_DEFAULT_APP_TITLE ${target} PXL_APP_TITLE)
    configure_file("${PXL_WEB_DIR}/redirect.html.in" "${PXL_DIST_DIR}/index.html" @ONLY)
endfunction()
