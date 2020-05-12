#include <iostream>
#include <string>
#include <Shlwapi.h>

#include "process.hpp"
#include "injection.hpp"

bool validateArguments(int argc, char** argv) {
	if (argc > 4 || argc < 4) {
		std::cout << "Invalid usage of LightInject." << std::endl << std::endl;
		std::cout << "All execution methods: " << std::endl << "(1) CreateRemoteThread [RECOMMENDED]\n(2) NtCreateThreadEx\n(3) QueueUserAPC" << std::endl << std::endl;
		std::cout << "Example usage: " << std::endl << "lightinject.exe processName.exe \"C:\\example directory\\example.dll\" 1" << std::endl;
		std::cout << "The example above will inject example.dll into processName.exe using CreateRemoteThread injection (method 1)." << std::endl << std::endl;
		return false;
	}
	else {
		if (isProcessRunning(getProcessId(argv[1])) == false) {
			std::cout << "The specified process is not running." << std::endl;
			return false;
		}
		/*if (PathFileExists(argv[2]) == false) {
			std::cout << "Invalid dll path. Make sure it exists." << std::endl;
			return false;
		}*/
		short injectionType = std::stoi(argv[3]);
		if (injectionType < 1 || injectionType > 3) {
			std::cout << "Invalid injection method. Run without any arguments to see injection methods." << std::endl;
			return false;
		}
	}
	return true;
}

int main(int argc, char** argv) {
	if (validateArguments(argc, argv) == false) return -1;

	short injectionType = std::stoi(argv[3]);
	DWORD processId = getProcessId(argv[1]);
	if (injectionType == 1) {
		if (createRemoteThread(processId, argv[2]) == false) {
			return -1;
		}
	}
	else if (injectionType == 2) {
		if (ntCreateThreadEx(processId, argv[2]) == false) {
			return -1;
		}
	}
	else if (injectionType == 3) {
		if (queueUserAPC(processId, argv[2]) == false) {
			return -1;
		}
	}

	return 0;
}