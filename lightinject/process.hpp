#pragma once
#include <Windows.h>
#include <TlHelp32.h>

DWORD getProcessId(const char* processName) {
	DWORD processId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 processEntry;
		processEntry.dwSize = sizeof(processEntry);

		if (Process32First(hSnap, &processEntry)) {
			do {
				if (!_stricmp(processEntry.szExeFile, processName)) {
					processId = processEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &processEntry));
		}
	}

	CloseHandle(hSnap);
	return processId;
}

BOOL isProcessRunning(DWORD processId) {
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, processId);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
}