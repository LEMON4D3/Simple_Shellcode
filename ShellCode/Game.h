#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdio>

class Game {
private:
	void getGamePID();
	void injectShellCode();

	uintptr_t getModuleBaseAddress(const wchar_t* moduleName);


protected:
	const wchar_t* gameName;

	DWORD gamePID;
	HANDLE gameHandle;

public:
	Game(const wchar_t* gameName);
	~Game();
};