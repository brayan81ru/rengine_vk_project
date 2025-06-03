// DisplayManager.cpp
#include "DisplayManager.h"
#include <stdexcept>

void DisplayManager::Initialize() {
    int displayCount = SDL_GetNumVideoDisplays();
    if (displayCount < 1) {
        throw std::runtime_error("No displays detected");
    }

    int modeCount = SDL_GetNumDisplayModes(0);
    for (int i = 0; i < modeCount; ++i) {
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, i, &mode);
        modes.push_back({
            mode.w,
            mode.h,
            mode.refresh_rate,
            mode.format
        });
    }
}

const std::vector<DisplayMode>& DisplayManager::GetAvailableModes(int displayIndex) {
    return modes;
}

DisplayMode DisplayManager::GetCurrentMode(int displayIndex) {
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(displayIndex, &mode);
    return { mode.w, mode.h, mode.refresh_rate, mode.format };
}

DisplayMode DisplayManager::GetDesktopMode(int displayIndex) {
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(displayIndex, &mode);
    return { mode.w, mode.h, mode.refresh_rate, mode.format };
}