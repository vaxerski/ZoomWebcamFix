﻿#pragma once

#define WIN32_LEAN_AND_MEAN             // Wyklucz rzadko używane rzeczy z nagłówków systemu Windows
// Pliki nagłówkowe systemu Windows
#include <windows.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>

#define uwu(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTING, LPVOID, DWORD, LPDWORD) CreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTING, LPVOID, DWORD, LPDWORD)
