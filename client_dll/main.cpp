#include "dll_declaration.hpp"
#include <Windows.h>
#include <memory>
extern "C" CLIENT_DLL_API float ComputeSqrt(float x)
{
    return sqrtf(x);
}


BOOL APIENTRY DllMain([[maybe_unused]] HMODULE hModule,
    [[maybe_unused]] DWORD  ul_reason_for_call,
    [[maybe_unused]] LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
