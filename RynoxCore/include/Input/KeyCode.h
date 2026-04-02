#pragma once

#include <cstdint>

// Key codes based on USB HID Usage Tables (https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
// This provides a consistent set of key codes across platforms, abstracting away platform-specific differences.

namespace Rynox
{
    using KeyCode = uint32_t;
    namespace Key
    {
        enum : KeyCode
        {
            None = 0,

            // Letters (HID 0x04–0x1D)
            A = 0x04,
            B, C, D, E, F, G, H, I, J, K, L, M,
            N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

            // Numbers (HID 0x1E–0x27)
            Num1 = 0x1E,
            Num2, Num3, Num4, Num5,
            Num6, Num7, Num8, Num9, Num0,

            // Control keys (HID 0x28–0x38)
            Enter = 0x28,
            Escape,
            Backspace,
            Tab,
            Space,

            Minus = 0x2D,       // - _
            Equals,             // = +
            LeftBracket,        // [ {
            RightBracket,       // ] }
            Backslash,          // \ |

            Semicolon = 0x33,   // ; :
            Apostrophe,         // ' "
            Grave,              // ` ~
            Comma,              // , 
            Period,             // . >
            Slash,              // / ?

            // Function keys (HID 0x3A–0x45)
            F1 = 0x3A,
            F2, F3, F4, F5, F6,
            F7, F8, F9, F10, F11, F12,

            // Navigation (HID 0x49–0x52)
            Insert = 0x49,
            Home,
            PageUp,
            Delete,
            End,
            PageDown,

            // Arrow keys (HID 0x4F–0x52)
            ArrowRight = 0x4F,
            ArrowLeft,
            ArrowDown,
            ArrowUp,

            // Numpad (HID 0x53–0x63)
            NumLock = 0x53,
            NumpadDivide,
            NumpadMultiply,
            NumpadMinus,
            NumpadPlus,
            NumpadEnter,
            Numpad1,
            Numpad2, Numpad3, Numpad4, Numpad5,
            Numpad6, Numpad7, Numpad8, Numpad9,
            Numpad0,
            NumpadPeriod,

            // Modifiers (HID 0xE0–0xE7)
            LeftCtrl = 0xE0,
            LeftShift,
            LeftAlt,
            LeftSuper,      // Win/Cmd
            RightCtrl,
            RightShift,
            RightAlt,
            RightSuper,

            // System (HID 0x39–0x48 partial)
            CapsLock = 0x39,
            PrintScreen,
            ScrollLock,
            Pause,

            Count
        };
    }
}