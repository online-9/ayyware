// Credits to Valve and Shad0w
#define _CRT_SECURE_NO_WARNINGS

#include "NetVars.h"
#include "Valve/ClientRecvProps.h"
#include "Valve/CRC32.h"
#include "Interfaces.h"

#define ALIGN_SIZE 8

CNetVars NetVars;

//
// Shitty string filler for dumping neatly
//
const char* AlignText(int align)
{
	static char buffer[256];
	int i = 0;
	for (i = 0; i < (align*ALIGN_SIZE); i++)
	{
		buffer[i] = ' ';
	}
	buffer[i + 1] = 0;
	return buffer;
}

//
// Starts processing all of the classes and netvars, storing their hashes
//
void CNetVars::Initialize()
{
	freopen("netv2.txt", "w", stdout);
	ClientClass* pClass = Interfaces::Client->GetAllClasses();

	if (!pClass)
		return;

	// Itterate through all of the classes
	while (pClass)
	{
		if (!pClass)
			break;

		AddRecursive(pClass->m_pRecvTable, 0);

		pClass = pClass->m_pNext;
	}
	freopen("CONOUT$", "w", stdout);
}

//
// CRC's and adds all the tables netvars, and recursively does the same for any children
//
void CNetVars::AddRecursive(RecvTable *pTable, int level)
{
	if (pTable->m_nProps <= 0)
		return;

#ifdef DUMP_NETVARS_TO_FILE
	if (level)
		printf("%s[Start of %s]\n", AlignText(level+1), pTable->m_pNetTableName);
	else
		printf("%s\n", pTable->m_pNetTableName);
#endif

	for (auto i = 0; i < pTable->m_nProps; ++i)
	{
		RecvProp* pProp = &pTable->m_pProps[i];
		if(!pProp) return;

		char szCRCBuffer[256];
		sprintf_s(szCRCBuffer, "%s%s", pTable->m_pNetTableName, pProp->m_pVarName);
		DWORD dwCRC = CRC32(szCRCBuffer, strlen(szCRCBuffer));

#ifdef DUMP_NETVARS_TO_FILE
		printf("%s%s [ 0x%x ] [ CRC: 0x%x ]\n", AlignText(level+1), pProp->m_pVarName, pProp->m_Offset, dwCRC);
#endif
		NetvarHashes[dwCRC] = pProp->m_Offset;

		if(pProp->m_pDataTable)
			AddRecursive(pProp->m_pDataTable, level+1);
	}

#ifdef DUMP_NETVARS_TO_FILE
	if (level)
		printf("%s[End of %s]\n", AlignText(level + 1), pTable->m_pNetTableName);
#endif
}

//
// Use this to get the offset of the netvar you want
//
DWORD CNetVars::GetOffset(DWORD dwCRC32)
{
	if (NetvarHashes.find(dwCRC32) == NetvarHashes.end())
	{
		return 0;
	}
	else
	{
		return NetvarHashes[dwCRC32];
	}
}