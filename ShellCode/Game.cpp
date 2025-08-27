#include "Game.h"

Game::Game(const wchar_t* gameName) : gameName(gameName) {
	
	this->getGamePID();
	this->injectShellCode();

}

Game::~Game() {
	CloseHandle(this->gameHandle);
}

void Game::getGamePID() {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);
	if (Process32FirstW(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szExeFile, this->gameName) == 0) {
				this->gamePID = entry.th32ProcessID;
				this->gameHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, this->gamePID);
				CloseHandle(snapshot);
				wprintf(L"Found %ls with PID %d\n", this->gameName, this->gamePID);
				return;
			}
		} while (Process32NextW(snapshot, &entry));
	}
	CloseHandle(snapshot);
	wprintf(L"Could not find %ls\n", this->gameName);
	system("pause");
	exit(0);

}

uintptr_t Game::getModuleBaseAddress(const wchar_t* moduleName) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->gamePID);
	MODULEENTRY32W entry;
	entry.dwSize = sizeof(entry);
	if (Module32FirstW(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szModule, moduleName) == 0) {
				CloseHandle(snapshot);
				wprintf(L"Found %ls at 0x%p\n", moduleName, entry.modBaseAddr);
				return (uintptr_t)entry.modBaseAddr;
			}
		} while (Module32NextW(snapshot, &entry));
	}
	wprintf(L"Could not find %ls\n", moduleName);
	CloseHandle(snapshot);
	system("pause");
	exit(0);
}

void Game::injectShellCode() {
	uintptr_t baseAddr = this->getModuleBaseAddress(L"ac_client.exe") & 0xFFFFFFFF;
	uintptr_t finalAddr = baseAddr + 0xC73EF;
	BYTE shellCode[] = {
		0xFF, 0x00
	};
	SIZE_T bytesWritten;

	if (WriteProcessMemory(this->gameHandle, (LPVOID)finalAddr, shellCode, sizeof(shellCode), &bytesWritten)) {
		wprintf(L"Wrote %zu bytes to 0x%p\n", bytesWritten, (LPVOID)finalAddr);
	}
	else {
		wprintf(L"Failed to write memory\n");
		system("pause");
		exit(0);
	}

}