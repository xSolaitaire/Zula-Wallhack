#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include "pattern.h"
bool pInit = 0;
#define MaxPlayer 16
#define ENABLE_WALLHACK 0x4000
template <typename type>
type read(const uintptr_t ptr) {
    if (!IsBadReadPtr(reinterpret_cast<void*>(ptr), sizeof(type)))
        return *reinterpret_cast<type*>(ptr);
    return type{};
}


void EntityLoop() {
    while (true)
    {
        for (int i = 0; i < MaxPlayer; i++)
        {
            DWORD pEnt = read<DWORD>((uintptr_t)EntityListAddr + 0x513 + (i * 0x4));
            if (pEnt == 0) continue;
            auto mdl = read<uint32_t>(pEnt + 0xC);
            if (mdl != NULL)
            {
                *(long*)((uint32_t)pEnt + 0x34) = (long)0x4000;
            }
        }
        Sleep(2000);
    }
}


PVOID Init() {
    if (!pInit)
    {
        EntityListAddr = ScanProc(EntityListArray, EntityList_Mask); 
        pInit = 1;
    }
    if (!EntityListAddr)
    {
        MessageBoxA(0, "Failed To Get Entity List", "", MB_OK);
    }
    std::thread(EntityLoop).detach();
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        _beginthreadex(0, 0, (_beginthreadex_proc_type)Init, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

