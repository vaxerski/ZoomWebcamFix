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
#include <time.h>

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
	srand(time(NULL));

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
	
	//removed file patch due to it breaking the screensharing

	std::string titles[12] = { "ZoomWebcamFix - We're over China.",
		"ZoomWebcamFix - Made with <3 by Vaxer",
		"ZoomWebcamFix - So you can go and troll people with your green screen online :)",
		"ZoomWebcamFix - It's a bird! It's a plane! No, it's just OBS.",
		"ZoomWebcamFix - Open source, so you can trust me. I think.",
		"ZoomWebcamFix - Works as advertised, so I can write those titles :)",
		"ZoomWebcamFix - Because Zoom's devs are incompetent.",
		"ZoomWebcamFix - I have the power of God and OBS on my side!",
		"ZoomWebcamFix - Because I, too, want to put an anime girl behind me.",
		"ZoomWebcamFix - Whoaa Kyle, nice room you have there!",
		"ZoomWebcamFix - Coding is fun, but only when you finish coding.",
		"ZoomWebcamFix - What is love..."};

	int title = std::rand() % 12;

	SetConsoleTitle(titles[title].c_str());

	std::string tips[5] = {"You can report issues or suggestions at github.",
		"If nothing works, there should be an update. If there is not, it should be in a while. I think.",
		"You can minimize this window, just don't close it.",
		"This window will close when zoom closes.",
		"To actually exit Zoom, click the arrow on the taskbar, right click the Zoom icon and hit exit."};

	int tip = std::rand() % 5;

	std::cout << "[tip] " << tips[tip] <<  "\n\n[+] Waiting for Zoom... \n\n";



	LPVOID addr;

	bool patched = false;
	bool doPatch = true;

	while (ZoomRunning() == false) {
		if (GetAsyncKeyState(VK_END)) goto end;
		//removed due to it breaking screensharing
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