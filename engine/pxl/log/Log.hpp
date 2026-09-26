#pragma once

#include <string_view>
#include <utility>

#include <fmt/format.h>

namespace pxl::log
{
    enum class Level
    {
        Info,
        Warn,
        Error
    };

    void Write(Level level, std::string_view message);

    template <typename... Args>
    void Error(fmt::format_string<Args...> format, Args&&... args)
    {
        Write(Level::Error, fmt::format(format, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void Warn(fmt::format_string<Args...> format, Args&&... args)
    {
        Write(Level::Warn, fmt::format(format, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void Info(fmt::format_string<Args...> format, Args&&... args)
    {
        Write(Level::Info, fmt::format(format, std::forward<Args>(args)...));
    }
}
