/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "Interfaces.h"
#include "Utilities.h"

/*

Search for "Custom user info value"

0FD477F8   8D4C24 1C        LEA ECX,DWORD PTR SS:[ESP+1C]                        ; Move's the convar into ECX
0FD477FC   E8 8FE3FCFF      CALL engine.0FD15B90                                 ; Calls the NET_SetConVar constructor
0FD47801   56               PUSH ESI                                             ; pushes the value
0FD47802   57               PUSH EDI                                             ; pushes the name
0FD47803   8D4C24 24        LEA ECX,DWORD PTR SS:[ESP+24]                        ; Puts the convar back into ECX
0FD47807   C74424 24 444713>MOV DWORD PTR SS:[ESP+24],engine.10134744            ; Fixes the first VTable
0FD4780F   C74424 28 804713>MOV DWORD PTR SS:[ESP+28],engine.10134780            ; Fixes the second VTable
0FD47817   E8 24100000      CALL engine.0FD48840                                 ; Calls the Init function
0FD4781C   A1 C4722510      MOV EAX,DWORD PTR DS:[102572C4]                      ; Move g_Engine into EAX
0FD47821   8D5424 1C        LEA EDX,DWORD PTR SS:[ESP+1C]                        ; Puts the convar in EDX
0FD47825   6A 00            PUSH 0
0FD47827   6A 00            PUSH 0
0FD47829   52               PUSH EDX
0FD4782A   8B88 94000000    MOV ECX,DWORD PTR DS:[EAX+94]                        ; ECX = g_Engine->m_NetChannel
0FD47830   8B01             MOV EAX,DWORD PTR DS:[ECX]                           ; Access netchannel vtable
0FD47832   FF90 A8000000    CALL DWORD PTR DS:[EAX+A8]                           ; Call NetChannel func 42 (SendNetMsg)
0FD47838   8D4C24 1C        LEA ECX,DWORD PTR SS:[ESP+1C]                        ; Put the concmd back into ECX
0FD4783C   E8 CFE2FCFF      CALL engine.0FD15B10                                 ; Call the destructor for the convar



*/

NET_SetConVar::NET_SetConVar(const char* name, const char* value)
{
	typedef void(__thiscall* SetConVarConstructor_t)(void*);
	static SetConVarConstructor_t pNET_SetConVar = (SetConVarConstructor_t)(Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x83\xE9\x04\xE9\x00\x00\x00\x00\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x56\x8B\xF1\xC7\x06\x00\x00\x00\x00\x8D\x4E\x08", "xxxx????xxxxxxxxxxxxxxxx????xxx")+19);
	pNET_SetConVar(this);

	typedef void(__thiscall* SetConVarInit_t)(void*, const char*, const char*);
	static SetConVarInit_t pNET_SetConVarInit = (SetConVarInit_t)Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x55\x8B\xEC\x56\x8B\xF1\x57\x83\x4E\x14\x01\x83\x7E\x0C\x00", "xxxxxxxxxxxxxxx");
	pNET_SetConVarInit(this, name, value);
}

NET_SetConVar::~NET_SetConVar()
{
	typedef void(__thiscall* SetConVarConstructor_t)(void*);
	static SetConVarConstructor_t pNET_SetConVar = (SetConVarConstructor_t)(Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x00\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x56\x8B\xF1\x57\x8D\x7E\x04\xC7\x07\x00\x00\x00\x00", "xxxxxxxxxxxxxxxxxxxxx????")+12);
	pNET_SetConVar(this);
}