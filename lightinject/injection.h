#pragma once

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

// CreateRemoteThread Injection
BOOL createRemoteThread(DWORD processId, LPCSTR dllPath) {
	if (!processId) return FALSE;

	HANDLE process = OpenProcess(CREATE_THREAD_ACCESS, FALSE, processId);

	if (!process) {
		std::cout << "Failed to open process (PID: " + std::to_string(processId) + ")." << std::endl;
		return FALSE;
	}

	LPVOID loadLibAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	LPVOID remoteString = (LPVOID)VirtualAllocEx(process, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(process, remoteString, (LPVOID)dllPath, strlen(dllPath) + 1, NULL);
	HANDLE thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibAddress, remoteString, NULL, NULL);

	WaitForSingleObject(thread, INFINITE);
	
	if (remoteString != NULL) VirtualFreeEx(process, remoteString, 0, MEM_RELEASE);
	if (thread != NULL) CloseHandle(thread);
	if (process != NULL) CloseHandle(process);

	return TRUE;
}

// NtCreateThreadEx Injection
struct NtCreateThreadExBuffer {
	ULONG Size;
	ULONG Unknown1;
	ULONG Unknown2;
	PULONG Unknown3;
	ULONG Unknown4;
	ULONG Unknown5;
	ULONG Unknown6;
	PULONG Unknown7;
	ULONG Unknown8;
};

typedef NTSTATUS(WINAPI* LPFUN_NtCreateThreadEx)(
	OUT PHANDLE hThread,
	IN ACCESS_MASK DesiredAccess,
	IN LPVOID ObjectAttributes,
	IN HANDLE ProcessHandle,
	IN LPTHREAD_START_ROUTINE lpStartAddress,
	IN LPVOID lpParameter,
	IN BOOL CreateSuspended,
	IN ULONG StackZeroBits,
	IN ULONG SizeOfStackCommit,
	IN ULONG SizeOfStackReserve,
	OUT LPVOID lpBytesBuffer
);

BOOL ntCreateThreadEx(DWORD processId, LPCSTR dllPath) {
	if (!processId) return FALSE;

	HANDLE remoteThread = NULL;
	HANDLE process = OpenProcess(CREATE_THREAD_ACCESS, FALSE, processId);

	if (!process) {
		std::cout << "Failed to open process (PID: " + std::to_string(processId) + ")." << std::endl;
		return FALSE;
	}

	LPVOID remoteString = (LPVOID)VirtualAllocEx(process, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(process, remoteString, (LPVOID)dllPath, strlen(dllPath) + 1, NULL);

	PTHREAD_START_ROUTINE loadLibAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	PTHREAD_START_ROUTINE ntCreateThreadExAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtCreateThreadEx");

	NtCreateThreadExBuffer buffer;
	memset(&buffer, 0, sizeof(NtCreateThreadExBuffer));

	LARGE_INTEGER dwTmp1 = { 0 };
	LARGE_INTEGER dwTmp2 = { 0 };

	if (ntCreateThreadExAddress) {
		buffer.Size = sizeof(struct NtCreateThreadExBuffer);
		buffer.Unknown1 = 0x10003;
		buffer.Unknown2 = 0x8;
		buffer.Unknown3 = (DWORD*)&dwTmp2;
		buffer.Unknown4 = 0;
		buffer.Unknown5 = 0x10004;
		buffer.Unknown6 = 4;
		buffer.Unknown7 = (DWORD*)&dwTmp1;
		buffer.Unknown8 = 0;

		LPFUN_NtCreateThreadEx funNtCreateThreadEx = (LPFUN_NtCreateThreadEx)ntCreateThreadExAddress;
		NTSTATUS status = funNtCreateThreadEx(
			&remoteThread,
			0x1FFFFF,
			NULL,
			process,
			loadLibAddress,
			(LPVOID)remoteString,
			FALSE,
			NULL,
			NULL,
			NULL,
			NULL
		);

		WaitForSingleObject(remoteThread, INFINITE);
	}

	if (remoteString != NULL) VirtualFreeEx(process, remoteString, 0, MEM_RELEASE);
	if (remoteThread != NULL) CloseHandle(remoteThread);
	if (process != NULL) CloseHandle(process);

	return TRUE;
}

// QueueUserAPC Injection
BOOL queueUserAPC(DWORD processId, LPCSTR dllPath) {
	if (!processId) return FALSE;

	HANDLE process = OpenProcess(CREATE_THREAD_ACCESS, FALSE, processId);

	if (!process) {
		std::cout << "Failed to open process (PID: " + std::to_string(processId) + ")." << std::endl;
		return FALSE;
	}

	LPVOID remoteString = (LPVOID)VirtualAllocEx(process, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	LPVOID loadLibAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

	WriteProcessMemory(process, remoteString, (LPVOID)dllPath, (lstrlen(dllPath) + 1) * sizeof(wchar_t), NULL);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	
	DWORD threadId = 0;
	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);

	BOOL result = Thread32First(snapshot, &threadEntry);
	while (result) {
		result = Thread32Next(snapshot, &threadEntry);
		if (result) {
			if (threadEntry.th32OwnerProcessID == processId) {
				threadId = threadEntry.th32ThreadID;
				HANDLE thread = OpenThread(THREAD_SET_CONTEXT, FALSE, threadId);

				if (thread) {
					DWORD result = QueueUserAPC((PAPCFUNC)loadLibAddress, thread, (ULONG_PTR)remoteString);
					CloseHandle(thread);
				}
			}
		}
	}

	if (snapshot != NULL) CloseHandle(snapshot);
	if (remoteString != NULL) VirtualFreeEx(process, remoteString, 0, MEM_RELEASE);
	if (process != NULL) CloseHandle(process);

	return TRUE;
}

// SetWindowsHookEx Injection
BOOL setWindowsHookEx(DWORD processId, LPCSTR dllPath, const char* processName) {
	if (!processId) return FALSE;

	DWORD threadId = getThreadId(processId);
	if (threadId == 0) std::cout << "Failed to get thread ID." << std::endl; return FALSE;

	HMODULE dll = LoadLibraryEx(dllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
	if (dll == NULL) std::cout << "Failed to load DLL." << std::endl; return FALSE;

	HOOKPROC address = (HOOKPROC)GetProcAddress(dll, "poc");
	if (address == NULL) std::cout << "Failed to find DLL exported function." << std::endl; return FALSE;

	HWND target = FindWindow(NULL, processName);
	GetWindowThreadProcessId(target, &processId);

	HHOOK handle = SetWindowsHookEx(WH_KEYBOARD, address, dll, threadId);
	if (handle == NULL) {
		std::cout << "Failed to hook KEYBOARD, SetWindowsHookEx." << std::endl;
		return FALSE;
	}
	else {
		UnhookWindowsHookEx(handle);
	}

	return TRUE;
}