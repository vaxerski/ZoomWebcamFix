#pragma once

#include <Psapi.h>
#include "framework.h"
#include <string>

//sig scanner by zer0mem0ry, thanks.

class cSigscan
{
public:
	MODULEINFO GetModuleInfo(char* szModule)
	{
		MODULEINFO modinfo = { 0 };
		HMODULE hModule = GetModuleHandle(szModule);
		if (hModule == 0) {
			std::string s = "Module not found: " + (std::string)szModule + " | error code: " + std::to_string(GetLastError());
			std::wstring w = std::wstring(s.begin(), s.end());
			MessageBoxW(NULL, w.c_str(), L":(", NULL);
			return modinfo;
		}

		GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
		return modinfo;
	}

	DWORD FindPattern(char* module, char* pattern, char* mask)
	{
		MODULEINFO mInfo = GetModuleInfo(module);
		if (mInfo.lpBaseOfDll == 0) {
			return 1;
		}
		DWORD base = (DWORD)mInfo.lpBaseOfDll;
		DWORD size = (DWORD)mInfo.SizeOfImage;
		DWORD patternLength = (DWORD)strlen(mask);

		for (DWORD i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (DWORD j = 0; j < patternLength; j++)
			{
				found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
			}
			if (found)
			{
				return base + i;
			}
		}

		return NULL;
	}
};

extern cSigscan* sigscan;