#include <pxl/platform/Page.hpp>

#include <string>

#include <emscripten/em_js.h>

#include <pxl/log/Log.hpp>

// clang-format off
EM_JS(void, pxl_show_fatal_error, (const char* message), {
    if (Module['pxlOnFatalError']) Module['pxlOnFatalError'](UTF8ToString(message));
});
// clang-format on

namespace pxl::platform
{
    void ShowFatalError(std::string_view message)
    {
        log::Error("fatal: {}", message);

        const std::string text(message);
        pxl_show_fatal_error(text.c_str());
    }
}
