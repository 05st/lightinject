#include <iostream>
#include <string>
#include <Shlwapi.h>

#include "process.hpp"
#include "injection.hpp"

bool validateArguments(int argc, char** argv) {
	if (argc > 4 || argc < 4) {
		std::cout << "Invalid usage of LightInject." << std::endl << std::endl;
		std::cout << "All execution methods: " << std::endl << "(1) CreateRemoteThread" << std::endl << std::endl;
		std::cout << "Example usage: " << std::endl << ".\\lightinject.exe processName.exe \"C:\\example directory\\example.dll 1\"" << std::endl;
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
		if (std::stoi(argv[3]) != 1) {
			std::cout << "Invalid injection method. Run without any arguments to see injection methods." << std::endl;
			return false;
		}
	}
	return true;
}

int main(int argc, char** argv) {
	if (validateArguments(argc, argv) == false) return -1;

	short injectionType = std::stoi(argv[3]);
	if (injectionType == 1) {
		if (createRemoteThread(getProcessId(argv[1]), argv[2]) == false) {
			// Message is already outputted by process.hpp, so exit with return code -1.
			return -1;
		}
	}

	return 0;
}