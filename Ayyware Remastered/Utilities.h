/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Common.h"


// Colors for the console
//Define extra colours (no clue where I found these)
#define FOREGROUND_WHITE		    (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_YELLOW       	(FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_CYAN		        (FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_MAGENTA	        (FOREGROUND_RED | FOREGROUND_BLUE)
#define FOREGROUND_BLACK		    0

#define FOREGROUND_INTENSE_RED		(FOREGROUND_RED | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_GREEN	(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_BLUE		(FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_WHITE	(FOREGROUND_WHITE | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_YELLOW	(FOREGROUND_YELLOW | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_CYAN		(FOREGROUND_CYAN | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_MAGENTA	(FOREGROUND_MAGENTA | FOREGROUND_INTENSITY)

//
// Ayy lmao big utils memes
//
namespace Utilities
{
	void OpenConsole();
	inline void CloseConsole() { FreeConsole(); }

	inline void SetConsoleColor(WORD color)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	}

	void Log(const char *fmt, ...);
	void EnableLogFile(std::string filename);

	std::string GetTimeString();

	// Memory utils
	// Purpose: Provides memeory related functionality (pattern scanning ect)
	namespace Memory
	{
		// Waits for a module to be available, before returning it's base address
		DWORD WaitOnModuleHandle(std::string moduleName);

		// Waits for a pointer to point to a non-zero value
		void WaitOnValidPointer(DWORD* Pointer);

		// Attempts to locate the given signature and mask in a memory range
		// Returns the address at which it has been found
		DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask);

		// Attempts to locate the given text in a memory range
		// Returns the address at which it has been found
		DWORD FindTextPattern(std::string moduleName, char* string);

		class VMTManager
		{
		private:
			DWORD* OriginalTable;
			DWORD* CustomTable;
			DWORD* Instance;

			int MethodCount();
		public:
			~VMTManager();

			bool	Initialise(DWORD* InstancePointer);
			DWORD	HookMethod(DWORD NewFunction, int Index);
			DWORD	GetOriginalFunction(int Index);
			void	Restore();
		};
	};
};
