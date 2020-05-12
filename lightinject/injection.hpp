#pragma once

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

BOOL createRemoteThread(DWORD processId, LPCSTR dllPath) {
	if (!processId) return FALSE;

	LPVOID loadLibAddress, remoteString;
	HANDLE process = OpenProcess(CREATE_THREAD_ACCESS, FALSE, processId);

	if (!process) {
		std::cout << "Failed to open process (PID: " + std::to_string(processId) + ")." << std::endl;
		return FALSE;
	}

	loadLibAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	remoteString = (LPVOID)VirtualAllocEx(process, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(process, remoteString, (LPVOID)dllPath, strlen(dllPath) + 1, NULL);
	HANDLE thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibAddress, remoteString, NULL, NULL);

	WaitForSingleObject(thread, INFINITE);
	
	if (remoteString != NULL) VirtualFreeEx(process, remoteString, 0, MEM_RELEASE);
	if (thread != NULL) CloseHandle(thread);
	if (process != NULL) CloseHandle(process);

	return TRUE;
}