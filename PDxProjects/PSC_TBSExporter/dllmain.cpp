// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
HINSTANCE handle_instance;
extern ClassDesc2* GetExportDesc();


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	handle_instance = hModule;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


__declspec(dllexport) const TCHAR* LibDescription()
{
	return _T("tbsExporter v1");
}
__declspec(dllexport) int LibNumberClasses()
{
	return 1;
}
__declspec(dllexport) ClassDesc* LibClassDesc(int i)
{
	switch (i)
	{
	case 0: return GetExportDesc();
	default: return 0;
	}
}
__declspec(dllexport) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}
__declspec(dllexport) int LibInitialize(void)
{
	return TRUE;
}
__declspec(dllexport) int LibShutdown(void)
{
	return TRUE;
}

