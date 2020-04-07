#pragma once
#include "framework.h"

class cModules {
public:
	cModules();

	const char* mDllSafeCheck = "DllSafeCheck.dll";
	DWORD hDllSafeCheck;

	const char* mDuiLib = "DuiLib.dll";
	DWORD hDuiLib;

	const char* mZoom = "Zoom.exe";
	DWORD hZoom;
};

extern cModules* modules;