#include <pxl/platform/Page.hpp>

#include <cstdio>
#include <string>

#include <emscripten/em_js.h>

// clang-format off
EM_JS(void, pxl_show_fatal_error, (const char* message), {
    if (Module['pxlOnFatalError']) Module['pxlOnFatalError'](UTF8ToString(message));
});
// clang-format on

namespace pxl::platform
{
    void ShowFatalError(std::string_view message)
    {
        std::fprintf(stderr, "[pxl] fatal: %.*s\n", static_cast<int>(message.size()), message.data());

        const std::string text(message);
        pxl_show_fatal_error(text.c_str());
    }
}
