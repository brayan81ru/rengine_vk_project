#include "RWindows.h"
#include <stdexcept>
#include <SDL_syswm.h>

namespace REngine {
    RWindows::RWindows(const std::string& title, int width, int height) {
        SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            throw std::runtime_error(SDL_GetError());
        }

        // Platform-optimized flags
        Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

        #if defined(__APPLE__)
        flags |= SDL_WINDOW_ALLOW_HIGHDPI;  // macOS Retina support
        #endif

        m_Window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            flags
        );

        if (!m_Window) {
            throw std::runtime_error(SDL_GetError());
        }

        m_quit = false;

        m_Initialized = true;
    }

    RWindows::~RWindows() {
        if (m_Window) SDL_DestroyWindow(m_Window);
        if (m_Initialized) SDL_Quit();
    }

    void RWindows::Run() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_Event = event;
            if (event.type == SDL_QUIT) {
                m_quit = true;
            }
        }
    }

    bool RWindows::IsRunning() const {
        return !m_quit;
    }

    const SDL_Event *RWindows::SDL_GetEvent() const {
        return &m_Event;
    }
}
