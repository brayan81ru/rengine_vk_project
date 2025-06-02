#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <string>
#include "NativeWindow.h"

namespace REngine {
    class RWindows {

    public:

        static Diligent::NativeWindow SDLWindowToNativeWindow(SDL_Window* window);

        RWindows(const std::string& title, int width, int height);

        ~RWindows();

        [[nodiscard]] SDL_Window* GetNativeWindow() const { return m_Window; }

        void Run();

        [[nodiscard]] bool IsRunning() const;

        SDL_Window* m_Window = nullptr;

        bool m_Initialized = false;

        const SDL_Event *SDL_GetEvent() const;
    private:
        bool m_quit = false;
        SDL_Event m_Event;
    };
}
