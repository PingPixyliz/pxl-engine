# pxl_embed_shaders(<target> <shader>...)
#   Embeds each WGSL file in <target> as a std::string_view constant, generated at build time:
#     shaders/triangle.wgsl -> #include "shaders/triangle.wgsl.hpp" -> shaders::triangle_wgsl
#   The symbol is the file name with non-identifier characters replaced by '_'.

include_guard(GLOBAL)

set(_PXL_EMBED_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/scripts/EmbedTextFile.cmake")

function(pxl_embed_shaders target)
    set(generated_root "${CMAKE_CURRENT_BINARY_DIR}/generated/${target}")
    set(generated_headers "")

    foreach(shader IN LISTS ARGN)
        cmake_path(ABSOLUTE_PATH shader BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE
                   OUTPUT_VARIABLE input)
        cmake_path(GET input FILENAME file_name)
        string(MAKE_C_IDENTIFIER "${file_name}" symbol)
        set(output "${generated_root}/shaders/${file_name}.hpp")

        add_custom_command(
            OUTPUT "${output}"
            COMMAND "${CMAKE_COMMAND}"
                    "-DINPUT=${input}"
                    "-DOUTPUT=${output}"
                    "-DNAMESPACE=shaders"
                    "-DSYMBOL=${symbol}"
                    -P "${_PXL_EMBED_SCRIPT}"
            DEPENDS "${input}" "${_PXL_EMBED_SCRIPT}"
            COMMENT "Embedding shader ${file_name}"
            VERBATIM
        )
        list(APPEND generated_headers "${output}")
    endforeach()

    # Listing the inputs as sources makes them show up in IDE project views.
    target_sources(${target} PRIVATE ${generated_headers} ${ARGN})
    target_include_directories(${target} PRIVATE "${generated_root}")
endfunction()
