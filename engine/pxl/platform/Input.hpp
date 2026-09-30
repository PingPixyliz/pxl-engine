#pragma once

#include <bitset>
#include <cstddef>
#include <string>
#include <string_view>

#include <glm/vec2.hpp>

#include <pxl/platform/Key.hpp>

namespace pxl::platform
{
    class Input
    {
        public:
            Input() = default;
            Input(const Input&) = delete;
            Input& operator=(const Input&) = delete;

            void Attach(const std::string& canvasSelector);
            void BeginFrame();

            bool IsKeyDown(Key key) const { return m_Keys.down[Index(key)]; }
            bool WasKeyPressed(Key key) const { return m_Keys.pressed[Index(key)]; }
            bool WasKeyReleased(Key key) const { return m_Keys.released[Index(key)]; }

            bool IsMouseButtonDown(MouseButton button) const { return m_MouseButtons.down[Index(button)]; }
            bool WasMouseButtonPressed(MouseButton button) const { return m_MouseButtons.pressed[Index(button)]; }
            bool WasMouseButtonReleased(MouseButton button) const { return m_MouseButtons.released[Index(button)]; }

            glm::vec2 GetMousePosition() const { return m_MousePosition; }
            glm::vec2 GetMouseDelta() const { return m_MouseDelta; }
            float GetWheelDelta() const { return m_WheelDelta; }
            std::string_view GetTextInput() const { return m_TextInput; }

            void HandleKey(Key key, bool down, bool repeat);
            void HandleText(std::string_view utf8);
            void HandleMouseButton(MouseButton button, bool down);
            void HandleMouseMove(const glm::vec2& position);
            void HandleWheel(float delta);
            void HandleBlur();

        private:
            template <size_t N>
            struct ButtonSet
            {
                    std::bitset<N> down;
                    std::bitset<N> pressed;
                    std::bitset<N> released;
            };
            using KeySet = ButtonSet<static_cast<size_t>(Key::Count)>;
            using MouseButtonSet = ButtonSet<static_cast<size_t>(MouseButton::Count)>;

            template <typename Enum>
            static size_t Index(Enum value)
            {
                return static_cast<size_t>(value);
            }

            KeySet m_Keys;
            KeySet m_PendingKeys;
            MouseButtonSet m_MouseButtons;
            MouseButtonSet m_PendingMouseButtons;
            glm::vec2 m_MousePosition{};
            glm::vec2 m_PendingMousePosition{};
            glm::vec2 m_MouseDelta{};
            float m_WheelDelta = 0.0f;
            float m_PendingWheel = 0.0f;
            std::string m_TextInput;
            std::string m_PendingTextInput;
    };
}
