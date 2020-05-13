#pragma once

#include "detours.hpp"

typedef BOOL(WINAPI* f_HackCheckDLLMAIN)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

class cHooks {
public:
	cHooks();
	bool doAsmHook(void* toHook, void* fFunction, int len);

	bool unregSafeCheck(void);

	DWORD dwDllMainSafeCheck; //\x83\x7C\x24\x00\x00\x75\x41 xxx??xx
};

extern cHooks* hooks;