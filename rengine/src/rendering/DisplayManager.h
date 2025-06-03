﻿#pragma once
#include <vector>
#include <SDL.h>

namespace REngine {

    struct DisplayMode {
        int width;
        int height;
        int refreshRate;
        Uint32 format;
    };

    struct FullScreenMode {
        enum Value {
            ExclusiveFullScreen,
            FullScreenWindow,
            Windowed,
            Count
        };

        Value value;
        // Implicit conversion
        constexpr FullScreenMode(Value v = ExclusiveFullScreen) : value(v) {}
        operator Value() const { return value; }

        // Disallow implicit conversions from other types
        explicit operator bool() = delete;

        // String conversion
        const char* ToString() const {
            static const char* names[] = {
                "Exclusive FullScreen",
                "FullScreen Window",
                "Maximized Window",
                "Windowed"
            };
            return (value < Count) ? names[value] : "Unknown";
        }

        // Optional bonus methods
        static constexpr size_t CountValues() { return Count; }
        static const char* GetName(const size_t index) {
            static const char* names[] = {
                "ExclusiveFullScreen",
                "FullScreenWindow",
                "MaximizedWindow",
                "Windowed"
            };
            return (index < Count) ? names[index] : "Unknown";
        }
    };

    class DisplayManager {
    public:
        static void Initialize();
        static const std::vector<DisplayMode>& GetAvailableModes(int displayIndex = 0);
        static DisplayMode GetCurrentMode(int displayIndex = 0);
        static DisplayMode GetDesktopMode(int displayIndex = 0);
        static int FindDisplayMode(int width, int height, int refreshRate);
        static bool ApplyDisplayMode(SDL_Window *window, const DisplayMode &mode, const FullScreenMode &fullScreenMode);
    private:
        static std::vector<DisplayMode> modes;  // Add this line
    };
}