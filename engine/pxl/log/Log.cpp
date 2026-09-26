#include <pxl/log/Log.hpp>

#include <string>

#include <emscripten/console.h>

namespace pxl::log
{
    void Write(Level level, std::string_view message)
    {
        const std::string line = fmt::format("[pxl] {}", message);
        switch (level)
        {
            case Level::Info:
                emscripten_console_log(line.c_str());
                break;
            case Level::Warn:
                emscripten_console_warn(line.c_str());
                break;
            case Level::Error:
                emscripten_console_error(line.c_str());
                break;
        }
    }
}
