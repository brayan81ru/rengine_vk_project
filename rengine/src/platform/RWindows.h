#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <string>
#include "BasicTypes.h"
#include "NativeWindow.h"

namespace REngine {
    class RWindows {
    public:

        static Diligent::NativeWindow SDLWindowToNativeWindow(SDL_Window* window);

        RWindows(const std::string& title, int width, int height);

        ~RWindows();

        SDL_Window* GetNativeWindow() const { return m_Window; }

        void PollEvents(bool& shouldQuit);

        SDL_Window* m_Window = nullptr;

        bool m_Initialized = false;
    private:

    };
}
