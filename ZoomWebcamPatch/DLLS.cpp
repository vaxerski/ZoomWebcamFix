#include <iostream>
#include <Windows.h>
#include "DLLS.h"

cModules::cModules() {
	hDllSafeCheck = (UINT_PTR)GetModuleHandle(mDllSafeCheck);
	hDuiLib = (UINT_PTR)GetModuleHandle(mDuiLib);
	hZoom = (UINT_PTR)GetModuleHandle(mZoom);
}