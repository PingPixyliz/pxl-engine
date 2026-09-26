#pragma once

#include <cstdint>
#include <string>

namespace pxl::platform
{
    class Canvas
    {
        public:
            Canvas() = default;
            explicit Canvas(std::string selector);

            bool UpdateSize();

            const std::string& GetSelector() const { return m_Selector; }
            uint32_t GetWidth() const { return m_Width; }
            uint32_t GetHeight() const { return m_Height; }

        private:
            std::string m_Selector;
            uint32_t m_Width = 0;
            uint32_t m_Height = 0;
    };
}
