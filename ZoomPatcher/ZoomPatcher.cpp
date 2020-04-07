#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <shellapi.h>
#include <ShlObj.h>
#include <ShlObj_core.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include "patchedDLL.h"


DWORD pids[16];
int lastPID = 0;

char* byteDLLdata;

bool checkPID(DWORD pid) {
	for (int i = 0; i < 16; ++i) {
		if (pid == pids[i]) return false;
	}
	return true;
}

bool compareFiles(const std::string& p1, const std::string& p2) {
	std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

	if (f1.fail() || f2.fail()) {
		return false; //file problem
	}

	if (f1.tellg() != f2.tellg()) {
		return false; //size mismatch
	}

	//seek back to beginning and use std::equal to compare contents
	f1.seekg(0, std::ifstream::beg);
	f2.seekg(0, std::ifstream::beg);
	return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
		std::istreambuf_iterator<char>(),
		std::istreambuf_iterator<char>(f2.rdbuf()));
}

DWORD getProcess(char* processName) {
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(procEntry);

	do {
		if (!strcmp(procEntry.szExeFile, processName) && checkPID(procEntry.th32ProcessID)) {
			DWORD dwPID = procEntry.th32ProcessID;
			CloseHandle(hPID);

			pids[lastPID] = dwPID;
			lastPID++;

			return dwPID;
		}
	} while (Process32Next(hPID, &procEntry));

	return NULL;
}

bool ZoomRunning() {
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(procEntry);

	do {
		if (!strcmp(procEntry.szExeFile, (char*)"Zoom.exe")) {
			DWORD dwPID = procEntry.th32ProcessID;
			CloseHandle(hPID);

			if (dwPID == 0) return false;
			return true;
		}
	} while (Process32Next(hPID, &procEntry));

	return false;
}

LPVOID inject(DWORD pid, char* dll) {
	char myDLL[MAX_PATH];

	GetFullPathNameA(dll, MAX_PATH, myDLL, NULL);

	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);

	if (hProcess == 0) {
		std::cout << "[-] Process with PID " << pids[0] << " is invalid, skipping..." << std::endl;
		return 0;
	}

	LPVOID allocatedMem = VirtualAllocEx(hProcess, NULL, sizeof(myDLL), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(hProcess, allocatedMem, myDLL, sizeof(myDLL), NULL);

	CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, allocatedMem, 0, 0);

	CloseHandle(hProcess);
	return allocatedMem;
}

struct stat info;

int main() {
	// declare variables
	const char* process = "Zoom.exe";
	const char* maindll = "ZoomWebcamPatch.dll";
	bool found = false;
	bool backup = true;

	TCHAR path[MAX_PATH];
	std::string s_patha;
	std::string bin;
	std::string newpath;

	if (ZoomRunning()) {
		std::cout << "[-] Please close Zoom before launching the patcher!\n";
		Sleep(1000);
		return ERROR;
	}

	std::cout << "[+] Waiting for Zoom... \n\n[!] You can press HOME to patch the file rather than runtime patching. You can learn more on GitHub.\n\n";

	LPVOID addr;

	bool patched = false;
	bool doPatch = true;

	while (ZoomRunning() == false) {
		if (GetAsyncKeyState(VK_END)) goto end;
		if (GetAsyncKeyState(VK_HOME) && !patched) {
			patched = true;

			std::cout << "[+] Local Patching SafeCheck... \n\n";

			if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path))) {
				s_patha = path;
				s_patha += "\\Zoom";

				for (int i = -1; i < 50; i++) {
					std::string test = s_patha + "\\bin";

					if (i >= 0) {
						test += "_";
						if (i >= 10) {
							test += std::to_string(i);
						}
						else {
							test += "0" + std::to_string(i);
						}
					}

					bin = test;

					test = test + "\\DllSafeCheck.dll";
					std::ifstream f(test.c_str());

					if (f.good()) {
						s_patha = test;
						found = true;
						break;
					}
				}

				if (!found) {
					MessageBox(NULL, "Local Zoom bin folder not found.", ":(", NULL);
					return ERROR;
				}
			}
			else {
				MessageBox(NULL, "Local Zoom bin folder not found.", ":(", NULL);
				return ERROR;
			}

			const char* cpath = s_patha.c_str();

			std::cout << "[+] Zoom bin path found at: " << bin << std::endl << std::endl;

			std::cout << "[+] Patching DLL at: " << cpath << std::endl;

			newpath = bin + "\\DllSafeCheck.dll.original";

			std::ifstream f(newpath.c_str());
			if (f.good()) {
				std::cout << "[-] Backup DLL found at: " << newpath << ", no backup will happen until that file is moved! \n\n[?] Do you still want to patch? (y/n)" << std::endl;
				backup = false;

				for (;; Sleep(1)) {
					if (GetAsyncKeyState(0x4E)) {
						doPatch = false;
						break;
					}
					if (GetAsyncKeyState(0x59)) {
						break;
					}
				}
			}
			else {
				std::cout << "[+] Backup saved to .original." << std::endl;
				rename(cpath, newpath.c_str());
			}

			if (doPatch) {
				std::ofstream patchDLL;
				patchDLL.open(cpath, std::ios_base::binary);
				patchDLL.write(reinterpret_cast<const char*>(patchedDLLbytes), 59224); //BYTES
				patchDLL.close();

				std::cout << "\n[+] Success! Patched DLL at: " << cpath << std::endl;

				Sleep(2000);
				return ERROR_SUCCESS;
			}

			goto end;
		}
		Sleep(1);
	}

	getProcess((char*)"Zoom.exe");

	std::cout << "[+] Got Zoom main process on PID " << pids[0] << std::endl;

	addr = inject(pids[0], (char*)maindll);

	std::cout << "[+] Injected the dll at " << addr << std::endl << std::endl;

	for (;; Sleep(1000)) {
		while (getProcess((char*)process) == NULL) {
			if (!ZoomRunning()) break;
			Sleep(200);
		}
		if (!ZoomRunning()) break;
		std::cout << "[+] New Zoom process detected with PID: " << pids[lastPID - 1] << std::endl;
		std::cout << "[+] Injecting patch dll into " << pids[lastPID - 1] << std::endl;
		LPVOID address = inject(pids[lastPID - 1], (char*)maindll);
		if (address == 0) break;
		std::cout << "[+] Dll injected into " << pids[lastPID - 1] << " at " << address << std::endl << std::endl;
	}

	std::cout << "[-] Zoom closed. " << std::endl << std::endl;

end:

	std::cout << "[+] Shutting down... " << std::endl << std::endl;

	Sleep(500);
	// done
	return 0;
}