include_guard(GLOBAL)

function(pxl_set_warnings target)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wold-style-cast
        -Wnull-dereference
        -Wimplicit-fallthrough
    )

    if(PXL_WARNINGS_AS_ERRORS)
        target_compile_options(${target} PRIVATE -Werror)
    endif()
endfunction()
