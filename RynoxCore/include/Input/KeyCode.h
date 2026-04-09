#pragma once

#include <cstdint>

// Key codes based on USB HID Usage Tables (https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
// This provides a consistent set across platforms, abstracting away platform-specific differences.

namespace Rynox
{
    using KeyCode = uint32_t;
    namespace Key
    {
        enum : KeyCode
        {
            None = 0,

            // Letters (HID 0x04–0x1D / 4-29)
            A = 0x04,
            B, C, D, E, F, G, H, I, J, K, L, M,
            N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

            // Numbers (HID 0x1E–0x27 / 30-39)
            Num1 = 0x1E, Num2, Num3, Num4, Num5,
            Num6, Num7, Num8, Num9, Num0,

            // Control keys (HID 0x28–0x38 / 40-56)
            Enter = 0x28,
            Escape,
            Backspace,
            Tab,
            SpaceBar,

            Minus,              // - _
            Equals,             // = +
            LeftBracket,        // [ {
            RightBracket,       // ] }
            Backslash,          // \ |
            NonUsHash,          // # ~
            Semicolon,          // ; :
            Apostrophe,         // ' "
            GraveAccent,        // ` ~
            Comma,              // , <
            Period,             // . >
            Slash,              // / ?

            // Function keys (HID 0x3A–0x45 / 58-69)
            F1, F2, F3, F4, F5, F6,
            F7, F8, F9, F10, F11, F12,

            // Navigation (HID 0x49–0x52 / 73-78)
            Insert,
            Home,
            PageUp,
            Delete,
            End,
            PageDown,

            // Arrow keys (HID 0x4F–0x52 / 79-82)
            ArrowRight,
            ArrowLeft,
            ArrowDown,
            ArrowUp,

            // Keypad (HID 0x53–0x63 / 83-99)
            NumLock,
            NumpadDivide,
            NumpadMultiply,
            NumpadMinus,
            NumpadPlus,
            NumpadEnter,
            Numpad1, Numpad2, Numpad3, Numpad4, Numpad5,
            Numpad6, Numpad7, Numpad8, Numpad9, Numpad0,
            NumpadPeriod,

            // System keys
            CapsLock = 0x39,
            PrintScreen = 0x46,
            ScrollLock = 0x47,
            Pause = 0x48,
            Application = 0x65,

            // Modifiers (HID 0xE0–0xE7 / 224-231)
            LeftCtrl = 0xE0,
            LeftShift,
            LeftAlt,
            LeftGUI,        // Win/Cmd
            RightCtrl,
            RightShift,
            RightAlt,
            RightGUI,       // Win/Cmd

            Count
        };
    }
}