#include "REngineCore.h"
#include <iostream>
#include <__msvc_ostream.hpp>


namespace REngine {
    REngineCore * REngineCore::Init() {
        std::cout << "Initializing REngineCore" << std::endl;
        RTime::Init();
        return new REngineCore();
    }
}
