#include "framework.h"
#include "hooks.h"
#include "DLLS.h"
#include "sigscan.h"

void __declspec(naked) bytePatchSafeCheck(void) {
	__asm {
		nop
		nop
		nop
		nop
		nop
	}
}

BOOL WINAPI hooked_SafeCheckDLLMAIN(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	f_HackCheckDLLMAIN oSafeCheckDLLMAIN = (f_HackCheckDLLMAIN)hooks->dwDllMainSafeCheck;

	return oSafeCheckDLLMAIN(hinstDLL, fdwReason, lpvReserved);
}

cHooks::cHooks() {
	//----------ADDRESSES----------//

	// Zoom v5.0.2 (24046.0510) kind of tested
	//dwHackCheckMain = sigscan->FindPattern((char*)modules->mZoom, (char*)"\x83\x7C\x24\x00\x00\x75\x41", (char*)"xxx??xx");
	dwDllMainSafeCheck = sigscan->FindPattern((char*)modules->mDllSafeCheck, (char*)"\x83\x7C\x24\x00\x00\x75\x41", (char*)"xxx??xx"); //removed cuz its stupid

	//-----------------------------//

	if (dwDllMainSafeCheck == 0) {

		std::string errormsg = "Sigscan failed! \n\ndwDllMainSafeCheck = " + 
			std::to_string(dwDllMainSafeCheck) +
			"\n\nReport the issue on GitHub. I won't see it on any other forums.";
		std::wstring w_errormsg = std::wstring(errormsg.begin(), errormsg.end());
		MessageBoxW(NULL, w_errormsg.c_str(), L":(", NULL);
	}

	//proffessionally no checks for NULL after here, the user has been notified and the program will exit by crashing lololol

	//----------ASM HOOKS----------//

	// not needed
	//doAsmHook((void*)dwHackCheckMain, &bytePatchSafeCheck, 5);

	//-----------------------------//

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	//------------ATTACH-----------//

	DetourAttach(&(LPVOID&)dwDllMainSafeCheck, &hooked_SafeCheckDLLMAIN);

	//-----------------------------//

	DetourTransactionCommit();
}

bool cHooks::unregSafeCheck() {
	hooked_SafeCheckDLLMAIN((HINSTANCE)modules->hDllSafeCheck, 0, NULL);
	return true;
}

bool cHooks::doAsmHook(void* toHook, void* fFunction, int len) {
	if (len < 5) {
		return false;
	}

	DWORD curProtection;

	if (VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection) == 0) {
		MessageBoxA(nullptr, std::system_error(::GetLastError(), std::system_category(), "VirtualProtect failed").what(), nullptr, MB_OK);
	}

	memset(toHook, 0x90, len);

	DWORD relAddr = ((DWORD)fFunction - (DWORD)toHook) - 5;

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relAddr;

	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return true;
}