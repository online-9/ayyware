/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#define _CRT_SECURE_NO_WARNINGS

#include "Utilities.h"

#include <fstream>
#include <Psapi.h>

bool FileLog = false;
std::ofstream logFile;

// Opens a debug console
void Utilities::OpenConsole()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}

// Outputs text to the console
void Utilities::Log(const char *fmt, ...)
{
	if (!fmt) return; //if the passed string is null return
	if (strlen(fmt) < 2) return;

	//Set up va_list and buffer to hold the params 
	va_list va_alist;
	char logBuf[256] = { 0 };

	//Do sprintf with the parameters
	va_start(va_alist, fmt);
	_vsnprintf(logBuf + strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
	va_end(va_alist);

	//Output to console
	if (logBuf[0] != '\0')
	{
		SetConsoleColor(FOREGROUND_INTENSE_GREEN);
		printf("[%s]", GetTimeString().c_str());
		SetConsoleColor(FOREGROUND_WHITE);
		printf(": %s\n", logBuf);
	}

	// Also outout to file if we need to
	if (FileLog)
	{
		logFile << logBuf << std::endl;
	}
}

// Enables writing all log calls to a file
void Utilities::EnableLogFile(std::string filename)
{
	logFile.open(filename.c_str());
	if (logFile.is_open())
		FileLog = true;
}

// Gets the current time as a string
std::string Utilities::GetTimeString()
{
	//Time related variables
	time_t current_time;
	struct tm *time_info;
	static char timeString[10];

	//Get current time
	time(&current_time);
	time_info = localtime(&current_time);

	//Get current time as string
	strftime(timeString, sizeof(timeString), "%I:%M%p", time_info);
	return timeString;
}

// --------Utilities Memory------------ //

DWORD Utilities::Memory::WaitOnModuleHandle(std::string moduleName)
{
	DWORD ModuleHandle = NULL;
	while (!ModuleHandle)
	{
		ModuleHandle = (DWORD)GetModuleHandle(moduleName.c_str());
		if (!ModuleHandle)
			Sleep(100);
	}
	return ModuleHandle;
}

void Utilities::Memory::WaitOnValidPointer(DWORD* Pointer)
{
	DWORD** ppTemp = (DWORD**)Pointer;
	if (*ppTemp == nullptr)
	{
		while (*ppTemp == nullptr)
		{
			Sleep(100);
		}
	}
}

// God knows who made these ancient peices of shit, but they get the job done
bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask)
{
	for (; *szMask; ++szMask, ++Mask, ++Data)
	{
		if (*szMask == 'x' && *Mask != *Data)
		{
			return false;
		}
	}
	return (*szMask) == 0;
}

DWORD Utilities::Memory::FindPattern(std::string moduleName, BYTE* Mask, char* szMask)
{
	DWORD Address = WaitOnModuleHandle(moduleName.c_str());
	MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
	DWORD Length = ModInfo.SizeOfImage;

	for (DWORD c = 0; c < Length; c += 1)
	{
		if (bCompare((BYTE*)(Address + c), Mask, szMask))
		{
			return (DWORD)(Address + c);
		}
	}
	return 0;
}

DWORD Utilities::Memory::FindTextPattern(std::string moduleName, char* string)
{
	DWORD Address = WaitOnModuleHandle(moduleName.c_str());
	MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
	DWORD Length = ModInfo.SizeOfImage;

	// GHETTO AS FUCK LOL
	int len = strlen(string);
	char* szMask = new char[len + 1];
	for (int i = 0; i < len; i++)
	{
		szMask[i] = 'x';
	}
	szMask[len] = '\0';

	for (DWORD c = 0; c < Length; c += 1)
	{
		if (bCompare((BYTE*)(Address + c), (BYTE*)string, szMask))
		{
			return (DWORD)(Address + c);
		}
	}
	return 0;
}

int Utilities::Memory::VMTManager::MethodCount()
{
	size_t Index = 0;

	while (!IsBadCodePtr((FARPROC)OriginalTable[Index]))
	{
		if (!IsBadCodePtr((FARPROC)OriginalTable[Index]))
		{
			Index++;
		}
	}
	return Index;
}

bool Utilities::Memory::VMTManager::Initialise(DWORD* InstancePointer)
{
	Instance = InstancePointer;
	OriginalTable = (DWORD*)*InstancePointer;

	size_t Index = MethodCount() * 4;

	CustomTable = new DWORD[Index];
	if (!CustomTable) return false;
	memcpy((void*)CustomTable, (void*)OriginalTable, Index);

	*InstancePointer = (DWORD)CustomTable;
	return true;
}

Utilities::Memory::VMTManager::~VMTManager()
{
	if (CustomTable)
	{
		free(CustomTable);
	}
}

DWORD	Utilities::Memory::VMTManager::HookMethod(DWORD NewFunction, int Index)
{
	if (CustomTable)
	{
		CustomTable[Index] = NewFunction;
		return OriginalTable[Index];
	}
	else
		return NULL;
}

DWORD	Utilities::Memory::VMTManager::GetOriginalFunction(int Index)
{
	return OriginalTable[Index];
}

void	Utilities::Memory::VMTManager::Restore()
{
	*Instance = (DWORD)OriginalTable;
}