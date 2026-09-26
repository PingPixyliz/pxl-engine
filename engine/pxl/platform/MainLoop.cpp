#include <pxl/platform/MainLoop.hpp>

#include <utility>

#include <emscripten/html5.h>

namespace pxl::platform
{
    namespace
    {
        std::function<bool(double)> g_Frame;

        bool OnAnimationFrame(double timeMs, void*)
        {
            return g_Frame(timeMs);
        }
    }

    void StartMainLoop(std::function<bool(double timeMs)> frame)
    {
        g_Frame = std::move(frame);
        emscripten_request_animation_frame_loop(&OnAnimationFrame, nullptr);
    }
}
