# Build-time script (cmake -P) that wraps a text file in a C++ header exposing its contents
# as a std::string_view constant:
#   cmake -DINPUT=<file> -DOUTPUT=<header> -DNAMESPACE=<namespace> -DSYMBOL=<name> -P EmbedTextFile.cmake

foreach(required IN ITEMS INPUT OUTPUT NAMESPACE SYMBOL)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "EmbedTextFile: missing -D${required}=...")
    endif()
endforeach()

set(delimiter "PXL_EMBED")

file(READ "${INPUT}" content)

string(FIND "${content}" ")${delimiter}\"" delimiter_position)
if(NOT delimiter_position EQUAL -1)
    message(FATAL_ERROR "EmbedTextFile: ${INPUT} contains the raw-string delimiter ')${delimiter}\"'")
endif()

cmake_path(GET INPUT FILENAME input_name)

# file(WRITE) rather than configure_file(): configure_file() would substitute WGSL's '@' attributes.
file(WRITE "${OUTPUT}"
"// Generated from ${input_name} by cmake/scripts/EmbedTextFile.cmake - DO NOT EDIT.
#pragma once

#include <string_view>

namespace ${NAMESPACE} {

inline constexpr std::string_view ${SYMBOL} = R\"${delimiter}(${content})${delimiter}\";

}  // namespace ${NAMESPACE}
")
