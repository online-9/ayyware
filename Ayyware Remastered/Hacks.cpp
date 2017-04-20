/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "Hacks.h"
#include "Interfaces.h"
#include "Render.h"

#include "Menu.h"

#include "ESP.h"
#include "RageBot.h"
#include "HitMarker.h"
#include "LegitBot.h"

CEsp ESP;
CHitMarker HitMarker;
CRageBot RageBot;
CLegitBot LegitBot;

//
// Sets up all the hacks and registers them
//
void Hacks::Initialize()
{
	HackManager.AddHack(&ESP);
	HackManager.AddHack(&RageBot);
	HackManager.AddHack(&HitMarker);
	HackManager.AddHack(&LegitBot);

	ConVar* nameVar = Interfaces::CVar->FindVar("name");
	if (nameVar)
	{
		*(int*)((DWORD)&nameVar->fnChangeCallback + 0xC) = 0;
		Utilities::Log("Name CVar Patched");
	}

	//---------------------
	HackManager.Setup();
}

void Hacks::DrawHacks()
{
	// Crosshair
	if (Menu::Window.VisualsTab.OtherCrosshair.GetState())
	{
		RECT View = Render::GetViewport();
		int MidX = View.right / 2;
		int MidY = View.bottom / 2;
		Render::Line(MidX - 8, MidY - 8, MidX + 8, MidY + 8, Color(0, 0, 0, 200));
		Render::Line(MidX + 8, MidY - 8, MidX - 8, MidY + 8, Color(0, 0, 0, 200));
		Render::Line(MidX - 4, MidY - 4, MidX + 4, MidY + 4, Color(0, 255, 0, 255));
		Render::Line(MidX + 4, MidY - 4, MidX - 4, MidY + 4, Color(0, 255, 0, 255));
	}

	if (Menu::Window.MiscTab.OtherServerLag.GetState())
	{
		Interfaces::Engine->ClientCmd_Unrestricted("status");
	}

	//---------------------
	HackManager.Draw();
}

void Hacks::MoveHacks(CUserCmd* pCmd)
{
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (pCmd->buttons & IN_JUMP && GetAsyncKeyState(VK_SPACE) && Menu::Window.MiscTab.OtherBhop.GetState())
	{
		int iFlags = pLocal->GetFlags();
		if (!(iFlags & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;
			
		
	}
	static bool bChange;
	if (GetAsyncKeyState(VK_SPACE) && Menu::Window.MiscTab.OtherAutoStrafe.GetState())
	{

		if (pCmd->mousedx < 0)
		{
			pCmd->sidemove = -400.f;
		}

		if (pCmd->mousedx > 0)
		{
			pCmd->sidemove = 400.f;
		}

	}
	// Airstuck
	int AirKey = Menu::Window.MiscTab.keyAirStuck.GetKey();
	if (AirKey >= 0 && GUI.GetKeyState(AirKey) && !(pCmd->buttons & IN_ATTACK))
	{
		pCmd->tick_count = 16777216;
	}

	//---------------------
	HackManager.Move(pCmd);
}


//
// Run the setup method on all the hacks
//
void CHackManager::Setup()
{
	for (auto Hack : HackList)
	{
		Hack->Setup();
	}
}

//
// Call this shit in our panel hooks to draw all the cheets
//
void CHackManager::Draw()
{
	pLocalInstance = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (pLocalInstance)
	{
		for (auto Hack : HackList)
		{
			Hack->Draw();
		}
	}
}

//
// Call this shit in our panel hooks to draw all the cheets
//
void CHackManager::Move(CUserCmd* pCmd)
{
	pLocalInstance = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (pLocalInstance)
	{
			for (auto Hack : HackList)
		{
			Hack->Move(pCmd);
		}
	}
}

//
// Registers a new hack to be managed
//
void CHackManager::AddHack(Hack* pHack)
{
	HackList.push_back(pHack);
}

// Da instancezz
CHackManager HackManager;