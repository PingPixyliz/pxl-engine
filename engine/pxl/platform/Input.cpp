#include <pxl/platform/Input.hpp>

#include <cstddef>
#include <string_view>
#include <utility>

#include <emscripten/em_js.h>
#include <emscripten/html5.h>

#include <pxl/log/Log.hpp>

// clang-format off
EM_JS(void, pxl_input_disable_context_menu, (const char* selector), {
    const element = document.querySelector(UTF8ToString(selector));
    if (element) element.addEventListener('contextmenu', (event) => event.preventDefault());
});
// clang-format on

namespace pxl::platform
{
    namespace
    {
        constexpr float k_PixelsPerWheelLine = 100.0f;
        constexpr float k_LinesPerWheelPage = 10.0f;

        Input& InputFrom(void* userData)
        {
            return *static_cast<Input*>(userData);
        }

        MouseButton MouseButtonFromDom(unsigned short button)
        {
            switch (button)
            {
                case 0: return MouseButton::Left;
                case 1: return MouseButton::Middle;
                case 2: return MouseButton::Right;
                default: return MouseButton::Count;
            }
        }

        float WheelLines(double delta, unsigned int deltaMode)
        {
            const auto value = static_cast<float>(delta);
            switch (deltaMode)
            {
                case DOM_DELTA_PIXEL: return value / k_PixelsPerWheelLine;
                case DOM_DELTA_LINE: return value;
                case DOM_DELTA_PAGE: return value * k_LinesPerWheelPage;
                default: return value;
            }
        }

        glm::vec2 CanvasPixels(const EmscriptenMouseEvent& event)
        {
            const auto ratio = static_cast<float>(emscripten_get_device_pixel_ratio());
            return {static_cast<float>(event.targetX) * ratio, static_cast<float>(event.targetY) * ratio};
        }

        bool OnKey(int eventType, const EmscriptenKeyboardEvent* event, void* userData)
        {
            const bool down = eventType == EMSCRIPTEN_EVENT_KEYDOWN;
            InputFrom(userData).HandleKey(KeyFromCode(event->code), down, event->repeat);
            if (down)
            {
                InputFrom(userData).HandleText(TextFromKey(event->key));
            }
            return false;
        }

        bool OnMouseButton(int eventType, const EmscriptenMouseEvent* event, void* userData)
        {
            const MouseButton button = MouseButtonFromDom(event->button);
            if (button == MouseButton::Count)
            {
                return false;
            }
            InputFrom(userData).HandleMouseButton(button, eventType == EMSCRIPTEN_EVENT_MOUSEDOWN);
            return true;
        }

        bool OnMouseMove(int, const EmscriptenMouseEvent* event, void* userData)
        {
            InputFrom(userData).HandleMouseMove(CanvasPixels(*event));
            return false;
        }

        bool OnWheel(int, const EmscriptenWheelEvent* event, void* userData)
        {
            InputFrom(userData).HandleWheel(WheelLines(event->deltaY, event->deltaMode));
            return true;
        }

        bool OnBlur(int, const EmscriptenFocusEvent*, void* userData)
        {
            InputFrom(userData).HandleBlur();
            return false;
        }

        void Check(EMSCRIPTEN_RESULT result, std::string_view event)
        {
            if (result != EMSCRIPTEN_RESULT_SUCCESS)
            {
                log::Error("input: registering {} failed ({})", event, result);
            }
        }
    }

    void Input::Attach(const std::string& canvasSelector)
    {
        const char* canvas = canvasSelector.c_str();
        const char* window = EMSCRIPTEN_EVENT_TARGET_WINDOW;

        Check(emscripten_set_keydown_callback(window, this, false, OnKey), "keydown");
        Check(emscripten_set_keyup_callback(window, this, false, OnKey), "keyup");
        Check(emscripten_set_mousedown_callback(canvas, this, false, OnMouseButton), "mousedown");
        Check(emscripten_set_mouseup_callback(window, this, false, OnMouseButton), "mouseup");
        Check(emscripten_set_mousemove_callback(canvas, this, false, OnMouseMove), "mousemove");
        Check(emscripten_set_wheel_callback(canvas, this, false, OnWheel), "wheel");
        Check(emscripten_set_blur_callback(window, this, false, OnBlur), "blur");
        pxl_input_disable_context_menu(canvas);
    }

    void Input::BeginFrame()
    {
        m_Keys = m_PendingKeys;
        m_PendingKeys.pressed.reset();
        m_PendingKeys.released.reset();

        m_MouseButtons = m_PendingMouseButtons;
        m_PendingMouseButtons.pressed.reset();
        m_PendingMouseButtons.released.reset();

        m_MouseDelta = m_PendingMousePosition - m_MousePosition;
        m_MousePosition = m_PendingMousePosition;

        m_WheelDelta = std::exchange(m_PendingWheel, 0.0f);
        m_TextInput = std::exchange(m_PendingTextInput, {});
    }

    void Input::HandleKey(Key key, bool down, bool repeat)
    {
        if (key == Key::Unknown)
        {
            return;
        }
        const size_t index = Index(key);
        if (down)
        {
            if (!repeat)
            {
                m_PendingKeys.pressed.set(index);
            }
            m_PendingKeys.down.set(index);
        }
        else
        {
            m_PendingKeys.released.set(index);
            m_PendingKeys.down.reset(index);
        }
    }

    void Input::HandleText(std::string_view utf8)
    {
        m_PendingTextInput += utf8;
    }

    void Input::HandleMouseButton(MouseButton button, bool down)
    {
        const size_t index = Index(button);
        if (down)
        {
            m_PendingMouseButtons.pressed.set(index);
            m_PendingMouseButtons.down.set(index);
        }
        else
        {
            m_PendingMouseButtons.released.set(index);
            m_PendingMouseButtons.down.reset(index);
        }
    }

    void Input::HandleMouseMove(const glm::vec2& position)
    {
        m_PendingMousePosition = position;
    }

    void Input::HandleWheel(float delta)
    {
        m_PendingWheel += delta;
    }

    void Input::HandleBlur()
    {
        m_PendingKeys.released |= m_PendingKeys.down;
        m_PendingKeys.down.reset();
        m_PendingMouseButtons.released |= m_PendingMouseButtons.down;
        m_PendingMouseButtons.down.reset();
    }
}
