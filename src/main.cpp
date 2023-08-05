#include <iostream>

#include "Engine.hpp"

// force laptop to use dedicated gpu
#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) uint32_t  NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

int main()
{
    Engine engine;
    engine.run();
    return 0;
}