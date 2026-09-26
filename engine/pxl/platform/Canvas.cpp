#include <pxl/platform/Canvas.hpp>

#include <algorithm>
#include <cmath>
#include <utility>

#include <emscripten/html5.h>

namespace pxl::platform
{
    Canvas::Canvas(std::string selector) : m_Selector(std::move(selector)) {}

    bool Canvas::UpdateSize()
    {
        double cssWidth = 0.0;
        double cssHeight = 0.0;
        emscripten_get_element_css_size(m_Selector.c_str(), &cssWidth, &cssHeight);
        const double pixelRatio = emscripten_get_device_pixel_ratio();

        const auto width = static_cast<uint32_t>(std::max(1.0, std::round(cssWidth * pixelRatio)));
        const auto height = static_cast<uint32_t>(std::max(1.0, std::round(cssHeight * pixelRatio)));
        if (width == m_Width && height == m_Height)
        {
            return false;
        }

        m_Width = width;
        m_Height = height;
        emscripten_set_canvas_element_size(m_Selector.c_str(), static_cast<int>(width), static_cast<int>(height));
        return true;
    }
}
