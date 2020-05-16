#pragma once
#include <Windows.h>
#include <TlHelp32.h>

DWORD getProcessId(const char* processName) {
	DWORD processId = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 processEntry;
		processEntry.dwSize = sizeof(processEntry);

		if (Process32First(snap, &processEntry)) {
			do {
				if (!_stricmp(processEntry.szExeFile, processName)) {
					processId = processEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(snap, &processEntry));
		}
	}

	CloseHandle(snap);
	return processId;
}

DWORD getThreadId(DWORD processId) {
	DWORD threadId = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (snap != INVALID_HANDLE_VALUE) {
		THREADENTRY32 threadEntry;
		threadEntry.dwSize = sizeof(threadEntry);

		if (Thread32First(snap, &threadEntry)) {
			do {
				if (threadEntry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(threadEntry.th32OwnerProcessID)) {
					if (threadEntry.th32OwnerProcessID == processId) {
						HANDLE thread = OpenThread(READ_CONTROL, FALSE, threadEntry.th32ThreadID);
						if (thread) {
							threadId = threadEntry.th32ThreadID;
							break;
						}
					}
				}
			} while (Thread32Next(snap, &threadEntry));
		}
	}

	CloseHandle(snap);
	return threadId;
}

BOOL isProcessRunning(DWORD processId) {
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, processId);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
}