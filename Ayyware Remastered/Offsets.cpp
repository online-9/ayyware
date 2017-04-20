/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "Offsets.h"
#include "Utilities.h"

namespace Offsets
{
	namespace SigScans
	{
		DWORD dwCalcPlayerView;
		DWORD dwSpreadVmt;
		DWORD dwGetCSWpnData;
		DWORD dwKeyValues_KeyValues;
		DWORD dwKeyValues_LoadFromBuffer;
	};
};

void Offsets::Init()
{
	
	

	SigScans::dwGetCSWpnData = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x57", "xxxxx????x????x");

	SigScans::dwCalcPlayerView = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x84\xC0\x75\x08\x57\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x06", "xxxxxxxx????xx");

	SigScans::dwKeyValues_KeyValues = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x68\x00\x00\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\x89\x45\xFC\xEB\x07\xC7\x45\x00\x00\x00\x00\x00\x8B\x03\x56", "x????xxx????xxxxxxx?????xxx");
	SigScans::dwKeyValues_KeyValues += 7; 
	SigScans::dwKeyValues_KeyValues = SigScans::dwKeyValues_KeyValues + *reinterpret_cast< PDWORD_PTR >(SigScans::dwKeyValues_KeyValues + 1) + 5;

	SigScans::dwKeyValues_LoadFromBuffer = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\xE8\x00\x00\x00\x00\x8A\xD8\xFF\x15\x00\x00\x00\x00\x84\xDB", "x????xxxx????xx");
	SigScans::dwKeyValues_LoadFromBuffer = SigScans::dwKeyValues_LoadFromBuffer + *reinterpret_cast< PDWORD_PTR >(SigScans::dwKeyValues_LoadFromBuffer + 1) + 5;


#ifdef AYYDBG
	Utilities::Log("Offsets Manager Ready");
#endif
}