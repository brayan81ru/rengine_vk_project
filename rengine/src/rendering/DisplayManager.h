#pragma once
#include <vector>
#include <SDL.h>

struct DisplayMode {
    int width;
    int height;
    int refreshRate;
    Uint32 format;
};

class DisplayManager {
public:
    static void Initialize();
    static const std::vector<DisplayMode>& GetAvailableModes(int displayIndex = 0);
    static DisplayMode GetCurrentMode(int displayIndex = 0);
    static DisplayMode GetDesktopMode(int displayIndex = 0);

private:
    static std::vector<DisplayMode> modes;  // Add this line
};