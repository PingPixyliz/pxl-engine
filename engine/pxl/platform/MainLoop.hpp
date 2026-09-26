#pragma once

#include <functional>

namespace pxl::platform
{
    void StartMainLoop(std::function<bool(double timeMs)> frame);
}
