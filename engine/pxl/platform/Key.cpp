#include <pxl/platform/Key.hpp>

#include <array>
#include <cstddef>
#include <utility>

namespace pxl::platform
{
    namespace
    {
        constexpr int Span(Key first, Key last)
        {
            return static_cast<int>(last) - static_cast<int>(first);
        }

        static_assert(Span(Key::A, Key::Z) == 'Z' - 'A');
        static_assert(Span(Key::Digit0, Key::Digit9) == '9' - '0');
        static_assert(Span(Key::Numpad0, Key::Numpad9) == '9' - '0');
        static_assert(Span(Key::F1, Key::F12) == 11);

        constexpr auto k_NamedKeys = std::to_array<std::pair<std::string_view, Key>>({
            {"F1", Key::F1},
            {"F2", Key::F2},
            {"F3", Key::F3},
            {"F4", Key::F4},
            {"F5", Key::F5},
            {"F6", Key::F6},
            {"F7", Key::F7},
            {"F8", Key::F8},
            {"F9", Key::F9},
            {"F10", Key::F10},
            {"F11", Key::F11},
            {"F12", Key::F12},
            {"Space", Key::Space},
            {"Enter", Key::Enter},
            {"Escape", Key::Escape},
            {"Tab", Key::Tab},
            {"Backspace", Key::Backspace},
            {"Delete", Key::Delete},
            {"Insert", Key::Insert},
            {"Home", Key::Home},
            {"End", Key::End},
            {"PageUp", Key::PageUp},
            {"PageDown", Key::PageDown},
            {"ArrowLeft", Key::ArrowLeft},
            {"ArrowRight", Key::ArrowRight},
            {"ArrowUp", Key::ArrowUp},
            {"ArrowDown", Key::ArrowDown},
            {"ShiftLeft", Key::ShiftLeft},
            {"ShiftRight", Key::ShiftRight},
            {"ControlLeft", Key::ControlLeft},
            {"ControlRight", Key::ControlRight},
            {"AltLeft", Key::AltLeft},
            {"AltRight", Key::AltRight},
            {"MetaLeft", Key::MetaLeft},
            {"MetaRight", Key::MetaRight},
            {"CapsLock", Key::CapsLock},
            {"Minus", Key::Minus},
            {"Equal", Key::Equal},
            {"BracketLeft", Key::BracketLeft},
            {"BracketRight", Key::BracketRight},
            {"Backslash", Key::Backslash},
            {"Semicolon", Key::Semicolon},
            {"Quote", Key::Quote},
            {"Backquote", Key::Backquote},
            {"Comma", Key::Comma},
            {"Period", Key::Period},
            {"Slash", Key::Slash},
            {"NumpadAdd", Key::NumpadAdd},
            {"NumpadSubtract", Key::NumpadSubtract},
            {"NumpadMultiply", Key::NumpadMultiply},
            {"NumpadDivide", Key::NumpadDivide},
            {"NumpadDecimal", Key::NumpadDecimal},
            {"NumpadEnter", Key::NumpadEnter},
        });

        Key KeyInRange(std::string_view code, std::string_view prefix, char first, char last, Key base)
        {
            if (code.size() != prefix.size() + 1 || !code.starts_with(prefix))
            {
                return Key::Unknown;
            }
            const char suffix = code.back();
            if (suffix < first || suffix > last)
            {
                return Key::Unknown;
            }
            return static_cast<Key>(static_cast<int>(base) + (suffix - first));
        }

        size_t CodePointLength(unsigned char lead)
        {
            if (lead < 0x80)
            {
                return 1;
            }
            if ((lead & 0xE0) == 0xC0)
            {
                return 2;
            }
            if ((lead & 0xF0) == 0xE0)
            {
                return 3;
            }
            if ((lead & 0xF8) == 0xF0)
            {
                return 4;
            }
            return 0;
        }
    }

    Key KeyFromCode(std::string_view code)
    {
        if (const Key key = KeyInRange(code, "Key", 'A', 'Z', Key::A); key != Key::Unknown)
        {
            return key;
        }
        if (const Key key = KeyInRange(code, "Digit", '0', '9', Key::Digit0); key != Key::Unknown)
        {
            return key;
        }
        if (const Key key = KeyInRange(code, "Numpad", '0', '9', Key::Numpad0); key != Key::Unknown)
        {
            return key;
        }
        for (const auto& [name, key] : k_NamedKeys)
        {
            if (name == code)
            {
                return key;
            }
        }
        return Key::Unknown;
    }

    std::string_view TextFromKey(std::string_view key)
    {
        if (key.empty())
        {
            return {};
        }
        const auto lead = static_cast<unsigned char>(key.front());
        const size_t length = CodePointLength(lead);
        if (length == 0 || key.size() != length || (length == 1 && lead < 0x20))
        {
            return {};
        }
        return key;
    }
}
