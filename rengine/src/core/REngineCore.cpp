#include "REngineCore.h"
#include <iostream>


namespace REngine {
    REngineCore * REngineCore::Init() {
        std::cout << "Initializing REngineCore" << std::endl;
        RTime::Init();
        return new REngineCore();
    }
}
