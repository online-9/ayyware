//
// Syn's "Storm" Cheat
// Started October 2015
//

// Source engine interface management



#pragma once

//uncomment me if you wish to dump netvars
#define DUMP_NETVARS_TO_FILE 

#include "Common.h"

struct RecvTable;

class CNetVars
{
public:
	void Initialize();
	DWORD GetOffset(DWORD dwCRC32); // Gets a netvar offset given it's crc
private:
	void AddRecursive(RecvTable *pTable, int level);
	std::map<DWORD, DWORD> NetvarHashes;
};

extern CNetVars NetVars;

// This stuff keeps things tidy :)

#define dwThis (DWORD)this
#define NETVAR(type,offset) *(type*)(dwThis + offset)
#define CNETVAR(type,offset) (type)(dwThis + offset)

//CNETVAR_FUNC - creates a netvar function in a class that returns value(s) [type=class type & return type] [name=function name] [netvar_crc=crc32 of netvar of choice]
#define CNETVAR_FUNC(type,name,netvar_crc) \
	type name() \
	{ \
		static DWORD dwObserverTarget=NetVars.GetOffset(netvar_crc); \
		return NETVAR(type,dwObserverTarget); \
	}

//CPNETVAR_FUNC - creates a netvar function in a class that returns a pointer [type=class type & return type] [name=function name] [netvar_crc=crc32 of netvar of choice]
#define CPNETVAR_FUNC(type,name,netvar_crc) \
	type name() \
	{ \
		static DWORD dwObserverTarget=NetVars.GetOffset(netvar_crc); \
		return CNETVAR(type,dwObserverTarget); \
	}