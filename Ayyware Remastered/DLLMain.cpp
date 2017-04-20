/*
	AyyWare 2 - Extreme Alien Technology
	By Syn
*/

#include "DLLMain.h"
#include "Utilities.h"
#include "Offsets.h"
#include "NetVars.h"
#include "Interfaces.h"
#include "Render.h"
#include "Hooks.h"
#include "Menu.h"
#include "Hacks.h"
void FixY(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;

	if (Menu::Window.HvHTab.AccuracyAngleFix.GetState())
	{
		bHasAA = ((*flPitch == 90.0f) || (*flPitch == 270.0f));
		bSpinbot = false;

		if (!bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
			&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f) && !bHasAA)
		{
			flYaw = vLast[((IClientEntity*)(pStruct))->GetIndex()].y;
			bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
		}
		else
		{
			if (bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
				&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f))
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
				bSpinbot = true;
			}
			else
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = false;
			}
		}

		vLast[((IClientEntity*)(pStruct))->GetIndex()].y = flYaw;


		bool bTmp = bJitterFix[((IClientEntity*)(pStruct))->GetIndex()];

		bJitterFix[((IClientEntity*)(pStruct))->GetIndex()] = (flYaw >= 180.0f && flYaw <= 360.0f);

		if (bTmp && (flYaw >= 0.0f && flYaw <= 180.0f))
		{
			flYaw += 359.0f;
		}
	}

	*(float*)(pOut) = flYaw;
}

// Simple fix for Fake-Down
void FixX(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	float* ang = (float*)pOut;
	*ang = pData->m_Value.m_Float;

	if (!Menu::Window.HvHTab.AccuracyAngleFix.GetState()) return;

	if (pData->m_Value.m_Float > 180.0f)
		*ang -= 360.0f;
	else if (pData->m_Value.m_Float < -180.0f)
		*ang += 360.0f;

	if (pData->m_Value.m_Float > 89.0f && pData->m_Value.m_Float < 91.0f)
		*ang -= 90.0f;
	else if (pData->m_Value.m_Float > -89.0f && pData->m_Value.m_Float < -91)
		*ang += 90.0f;
}
void ApplyAAAHooks()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, "DT_CSPlayer"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Pitch Fix
				if (!strcmp(name, "m_angEyeAngles[0]"))
				{
					pProp->m_ProxyFn = FixX;
				}

				// Yaw Fix
				if (!strcmp(name, "m_angEyeAngles[1]"))
				{
					//Utilities::Log("Yaw Fix Applied");
					pProp->m_ProxyFn = FixY;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}
void CenteredTextPrint(std::string Text)
{
	std::string NewString;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns;


	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

	int TextLength = Text.length();
	int Space = (columns / 2) - (TextLength / 2);

	for (int i = 0; i < Space; i++)
	{
		NewString += " ";
	}

	NewString += Text;

	while (NewString.length() < columns)
	{
		NewString += " ";
	}

	printf("%s\n", NewString.c_str());
}


// -----------------------------------------------------------
// A thread to run all of the setup related stuff of the 
//  entire cheat. Either called from DLLMain or from our export
//
DWORD WINAPI InitThread(LPVOID lpThreadParam)
{

	// Debug Console Stuff
//#ifdef AYYDBG
	Utilities::OpenConsole();
	Utilities::SetConsoleColor(BACKGROUND_GREEN | BACKGROUND_INTENSITY);
	CenteredTextPrint("Ayyware 2.0");
	Utilities::SetConsoleColor(FOREGROUND_INTENSE_WHITE);
	Utilities::Log("Utilities Library Ready");
//#endif
	int backend = 1;
	if (backend=1)
	{
		Offsets::Init();
		Interfaces::Initialize();
		NetVars.Initialize();
		Render::Initialise();
		Menu::Initialize();
		Hacks::Initialize();
		Hooks::Apply();
		ApplyAAAHooks();
		Utilities::Log("Ready");
	}

	return 0;
}

// -----------------------------------------------------------
// Allows the loader to supply some information about the user
//
DLL_EXPORT DWORD WINAPI PhoneHome(LoaderInfo* loaderInfo)
{
	// Copy over the loader info
	if (loaderInfo)
	{
		BackEndInfo::g_LoaderInfoSet = true;
		memcpy(&BackEndInfo::g_LoaderInfo, loaderInfo, sizeof(LoaderInfo));
	}
	return 0;
}

// -----------------------------------------------------------
// DLL Entry Point
//
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		BackEndInfo::g_hModule = hinstDLL;

		BackEndInfo::g_LoaderInfoSet = false;
		//Ow you find the antipasta thats a good start.
		//CreateThread(Hack, Aimbot, InitThread, 451, 8, 12);

		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

// -----------------------------------------------------------
// Allows the cheat to access some of the info we aquire from
// the loader and the injection process
//
namespace BackEndInfo
{
	bool g_LoaderInfoSet;
	LoaderInfo g_LoaderInfo;
	HINSTANCE g_hModule;
	bool g_bUnloadModule;
};
