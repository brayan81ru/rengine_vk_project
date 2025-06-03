#include "DisplayManager.h"
#include <stdexcept>

namespace REngine {
    // Initialize the static member
    std::vector<DisplayMode> DisplayManager::modes;

    void DisplayManager::Initialize() {
        if (const int displayCount = SDL_GetNumVideoDisplays(); displayCount < 1) {
            throw std::runtime_error("No displays detected");
        }

        modes.clear();  // Clear existing modes

        int modeCount = SDL_GetNumDisplayModes(0);
        for (int i = 0; i < modeCount; ++i) {
            SDL_DisplayMode mode;
            if (SDL_GetDisplayMode(0, i, &mode)) {
                throw std::runtime_error(SDL_GetError());
            }
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
        if (SDL_GetCurrentDisplayMode(displayIndex, &mode)) {
            throw std::runtime_error(SDL_GetError());
        }
        return { mode.w, mode.h, mode.refresh_rate, mode.format };
    }

    DisplayMode DisplayManager::GetDesktopMode(int displayIndex) {
        SDL_DisplayMode mode;
        if (SDL_GetDesktopDisplayMode(displayIndex, &mode)) {
            throw std::runtime_error(SDL_GetError());
        }
        return { mode.w, mode.h, mode.refresh_rate, mode.format };
    }

    int DisplayManager::FindDisplayMode(const int width, const int height, const int refreshRate) {
        for (int i = 0; i < modes.size(); ++i) {
            if (modes[i].width == width && modes[i].height == height && modes[i].refreshRate == refreshRate) {
                return i;
            }
        }

        return -1;
    }
}
