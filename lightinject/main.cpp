#pragma once
#include <iostream>
#include <Shlwapi.h>
#include "process.hpp"

bool validateArguments(int argc, char** argv) {
	if (argc > 3 || argc < 3) {
		std::cout << "Invalid usage of LightInject. Correct usage: " << std::endl << ".\lightinject.exe processName.exe C:\\directory\\example.dll" << std::endl;
		return false;
	}
	else {
		if (isProcessRunning(getProcessId(argv[1])) == false) {
			std::cout << "The specified process is not running." << std::endl;
			return false;
		}
		if (PathFileExists(argv[2]) == false) {
			std::cout << "Invalid dll path. Make sure it exists." << std::endl;
			return false;
		}
	}
	return true;
}

int main(int argc, char** argv) {
	if (validateArguments(argc, argv) == false) return -1;

	return 0;
}