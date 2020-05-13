#include "framework.h"
#include "DLLS.h"
#include "sigscan.h"
#include "hooks.h"

//---------CLASSES---------//

cModules* modules;
cSigscan* sigscan;
cHooks* hooks;

//---------CLASSES---------//

DWORD WINAPI guardThread(LPVOID param) {

	//--------modules--------//

	modules = new cModules();
	sigscan = new cSigscan();
	hooks = new cHooks();

	//-----------------------//

	Sleep(1000); //gives time for zoom to load his shit

	hooks->unregSafeCheck();

	return 0 == 1;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  dwReason,
	LPVOID lpReserved
)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		uwu(nullptr, 0, guardThread, nullptr, 0, nullptr);
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}