/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "Utilities.h"
#include "Interfaces.h"
#include "Hooks.h"
#include "Render.h"
#include "Menu.h"
#include "Hacks.h"
#include "Chams.h"
#include "SpamManager.h"

// Funtion Typedefs
typedef void(__thiscall* DrawModelEx_)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);
typedef void(__thiscall* PaintTraverse_)(PVOID, unsigned int, bool, bool);
typedef bool(__thiscall* InPrediction_)(PVOID);
typedef void(__stdcall *FrameStageNotifyFn)(ClientFrameStage_t);
typedef void(__thiscall* RunCommand_)(PVOID, IClientEntity*, CUserCmd*, IMoveHelper*);

// Function Pointers to the originals
PaintTraverse_ oPaintTraverse;
DrawModelEx_ oDrawModelExecute;
FrameStageNotifyFn oFrameStageNotify;

// Hook function prototypes
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
bool __stdcall Hooked_InPrediction();
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
bool __fastcall CreateMoveClient_Hooked(void* self, int edx, float frametime, CUserCmd* pCmd);
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
void __stdcall Hooked_RunCommand(IClientEntity* pPlayer, CUserCmd* pCmd, IMoveHelper* pMoveHelper);

// VMT Managers
namespace Hooks
{
	// VMT Managers
	Utilities::Memory::VMTManager VMTPanel; // Hooking drawing functions
	Utilities::Memory::VMTManager VMTClient; // Maybe CreateMove
	Utilities::Memory::VMTManager VMTClientMode; // CreateMove for functionality
	Utilities::Memory::VMTManager VMTModelRender; // DrawModelEx for chams
	Utilities::Memory::VMTManager VMTPrediction; // InPrediction for no vis recoil
};

// S p i c y
void Hooks::Apply()
{
	VMTPrediction.Initialise((DWORD*)Interfaces::Prediction);
	VMTPrediction.HookMethod((DWORD)&Hooked_InPrediction, Offsets::VMT::Predict_InPrediction);// No Vis Recoil
	VMTPrediction.HookMethod((DWORD)&Hooked_RunCommand, 19); // To get the IMoveHelper

	// Panel hooks for drawing to the screen via surface functions
	VMTPanel.Initialise((DWORD*)Interfaces::Panels);
	oPaintTraverse = (PaintTraverse_)VMTPanel.HookMethod((DWORD)&PaintTraverse_Hooked, Offsets::VMT::Panel_PaintTraverse);

	// Chams
	VMTModelRender.Initialise((DWORD*)Interfaces::ModelRender);
	oDrawModelExecute = (DrawModelEx_)VMTModelRender.HookMethod((DWORD)&Hooked_DrawModelExecute, Offsets::VMT::ModelRender_DrawModelExecute);

	// Setup ClientMode Hooks
	VMTClientMode.Initialise((DWORD*)Interfaces::ClientMode);
	VMTClientMode.HookMethod((DWORD)&CreateMoveClient_Hooked, 24);

	// Meme
	VMTClient.Initialise((DWORD*)Interfaces::Client);
	oFrameStageNotify = (FrameStageNotifyFn)VMTClient.HookMethod( (DWORD)&Hooked_FrameStageNotify, 36);

#ifdef AYYDBG
	Utilities::Log("Hooks Applied");
#endif
}

//---------------------------------------------------------------------------------------------------------
//                                         Hooked Functions
//---------------------------------------------------------------------------------------------------------

// Knife Changer NetVar Hook
void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	// Get the knife view model id's
	int default_t		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iBayonet		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	int iButterfly		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iFlip			= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
	int iGunGame		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");
	int iGut			= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
	int iKarambit		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	int iFalchion		= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger			= Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");

	// Get local player (just to stop replacing spectators knifes)
	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (pLocal)
	{
		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->IsAlive() && (pData->m_Value.m_Int == default_t || pData->m_Value.m_Int == default_ct))
		{
			// Set whatever knife we want
			switch (Menu::Window.MiscTab.SkinKnife.GetIndex())
			{
			case 0:
				break;
			case 1:
				pData->m_Value.m_Int = iBayonet;
				break;
			case 2:
				pData->m_Value.m_Int = iButterfly;
				break;
			case 3:
				pData->m_Value.m_Int = iFlip;
				break;
			case 4:
				pData->m_Value.m_Int = iGunGame;
				break;
			case 5:
				pData->m_Value.m_Int = iGut;
				break;
			case 6:
				pData->m_Value.m_Int = iKarambit;
				break;
			case 7:
				pData->m_Value.m_Int = iM9Bayonet;
				break;
			case 8:
				pData->m_Value.m_Int = iHuntsman;
				break;
			case 9:
				pData->m_Value.m_Int = iFalchion;
				break;
			case 10:
				pData->m_Value.m_Int = iDagger;
				break;
			}
			
		}
	}

	// Carry on the to original proxy
	*(int*)((DWORD)pOut) = pData->m_Value.m_Int;
}
static int CurrentStage[65];
void FixAngel1(IClientEntity* pEnt, float* pfPitch, float* pfYaw)
{
	int index = pEnt->GetIndex();
	if (pfPitch) {
		if (CurrentStage[index] < 4)
			*pfPitch = 89.f;
		else if (CurrentStage[index] < 8)
			*pfPitch = -89.f;
	}
}
void FixAngel2(IClientEntity* pEnt, float* pfPitch, float* pfYaw)
{
	int index = pEnt->GetIndex();
	if (pfYaw) {
		static float SpinYaw[65], TwitchTime[65];
		static bool Twitch[65];
		SpinYaw[index] += Interfaces::Globals->frametime * 135.f;
		if (SpinYaw[index] > 360.f) SpinYaw[index] = 0.f;

		TwitchTime[index] -= Interfaces::Globals->frametime;
		if (TwitchTime[index] <= 0.f) {
			Twitch[index] = !Twitch[index];
			TwitchTime[index] = 0.02f;
		}

		*pfYaw = SpinYaw[index] - (Twitch[index] ? 180.f : 0.f);
	}
}


int GetEstimatedServerTickCount(float latency)
{
	return (int)floorf((float)((float)(latency) / (float)((uintptr_t)&Interfaces::Globals->interval_per_tick)) + 0.5) + 1 + (int)((uintptr_t)&Interfaces::Globals->tickcount);
}
float oldlowerbodyyaw;
// Hooked FrameStageNotify for removing visual recoil
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	oFrameStageNotify(curStage);

	if (!Interfaces::Engine->IsInGame()) return;

	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		/*for (auto i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
		{
			IClientEntity* pEnt;
			if (!(pEnt = Interfaces::EntList->GetClientEntity(i)))
				continue;
			if ((pEnt == HackManager.pLocal()))
				continue;
			player_info_t pTemp;

			if (!Interfaces::Engine->GetPlayerInfo(i, &pTemp))
				continue;
			float *Pitch = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEnt) + pEnt->EyeAnglesX());
			float *Yaw = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEnt) + pEnt->EyeAnglesY());
			float *FYaw = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEnt) + pEnt->LowerBodyYaw());
			if (pEnt->GetHealth())
			{
				if (Menu::Window.HvHTab.AccuracyAngleFix.GetState())
				{
					//float curtime = Interfaces::Globals->curtime;
					Yaw = FYaw;
					if (*Pitch == 180.f) *Pitch = 89.f;
					else if (*Pitch > 180.f && *Pitch < 290.f) *Pitch = -89.f;
					else if (*Pitch >= 89.f && *Pitch <= 180.f) *Pitch = 89.f;
					else if (*Pitch == 0.f)
						FixAngel1(pEnt, Pitch, 0);
					//static Vector angles = Vector(0, 0, 0);
					//static float difference = 0;

					/*float PlayerIsMoving = abs(pEnt->GetVelocity().Length());
					bool bLowerBodyUpdated = false;
					bool IsUsingFakeAngles = false;

					if (oldlowerbodyyaw != *FYaw)
					{
						bLowerBodyUpdated = true;
					}
					float bodyeyedelta = pEnt->GetEyeAngles().y - *Yaw;

					if (PlayerIsMoving || bLowerBodyUpdated)// || LastUpdatedNetVars->eyeangles.x != CurrentNetVars->eyeangles.x || LastUpdatedNetVars->eyeyaw != CurrentNetVars->eyeangles.y)
					{
						if (bLowerBodyUpdated || (PlayerIsMoving && bodyeyedelta >= 35.0f))
						{
							*Yaw = *FYaw;
							oldlowerbodyyaw = *Yaw;
						}

						IsUsingFakeAngles = false;
					}
					else
					{
						if (bodyeyedelta > 35.0f)
						{
							*Yaw = oldlowerbodyyaw;
							IsUsingFakeAngles = true;
						}
						else
						{
							IsUsingFakeAngles = false;
						}
					}
					if (IsUsingFakeAngles)
					{
						int com = GetEstimatedServerTickCount(90);

						if (com % 2)
						{
							*Yaw += 90;
						}
						else if (com % 3)
							*Yaw -= 90;
						else
							*Yaw -= 0;
					}*/
				//}
			//}
		//}
		if (Interfaces::Engine->GetLocalPlayer() >= 0)
		{
			IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
			if (pLocal == nullptr) return;
			if (!pLocal->IsAlive()) return;

			for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
			{
				CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntity(i);
				if (pWeapon == nullptr) continue;
				ClientClass* cClass = ((IClientEntity*)pWeapon)->GetClientClass();

				if (cClass->m_ClassID != (int)CSGOClassID::CBaseWeaponWorldModel && ((strstr(cClass->m_pNetworkName, "Weapon") || cClass->m_ClassID == (int)CSGOClassID::CDEagle || cClass->m_ClassID == (int)CSGOClassID::CAK47) || cClass->m_ClassID == (int)CSGOClassID::CKnife))
				{
					if (pWeapon->m_AttributeManager() == nullptr) continue;
					if (pWeapon->m_AttributeManager()->m_Item() == nullptr) continue;
					if (Interfaces::EntList->GetClientEntityFromHandle(pWeapon->GetOwnerHandle()) != pLocal) continue;

					if (pWeapon->m_AttributeManager()->m_Item()->ItemIDHigh())
					{
						*pWeapon->m_AttributeManager()->m_Item()->ItemIDHigh() = 1;
					}

					*pWeapon->OriginalOwnerXuidHigh() = 0;
					*pWeapon->OriginalOwnerXuidLow() = 0;
					*pWeapon->m_AttributeManager()->m_Item()->AccountID() = 1;
					*pWeapon->m_AttributeManager()->m_Item()->EntityQuality() = 0;


					if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_DEAGLE)
					{
						*pWeapon->FallbackPaintKit() = 37;
						memcpy_s(pWeapon->m_AttributeManager()->m_Item()->szCustomName(), 40, "420 Blaze It", 40);
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_M4A1)
					{
						*pWeapon->FallbackPaintKit() = 497;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_AK47)
					{
						*pWeapon->FallbackPaintKit() = 474;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_AWP)
					{
						*pWeapon->FallbackPaintKit() = 344;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_USP)
					{
						*pWeapon->FallbackPaintKit() = 504;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_GLOCK)
					{
						*pWeapon->FallbackPaintKit() = 437;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_FIVE7)
					{
						*pWeapon->FallbackPaintKit() = 374;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_TEC9)
					{
						*pWeapon->FallbackPaintKit() = 352;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_SSG08)
					{
						*pWeapon->FallbackPaintKit() = 222;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_P90)
					{
						*pWeapon->FallbackPaintKit() = 156;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_M4A4)
					{
						*pWeapon->FallbackPaintKit() = 309;
						*pWeapon->FallbackWear() = 0.00000001;
						memcpy_s(pWeapon->m_AttributeManager()->m_Item()->szCustomName(), 40, "Doge xDDDD", 40);
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_KNIFECT || *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_KNIFET)
					{
						*pWeapon->FallbackPaintKit() = 38;
						*pWeapon->FallbackWear() = 0.00000001;
						memcpy_s(pWeapon->m_AttributeManager()->m_Item()->szCustomName(), 40, "Ayyware 2.0", 40);
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_FAMAS)
					{
						*pWeapon->FallbackPaintKit() = 154;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_FAMAS)
					{
						*pWeapon->FallbackPaintKit() = 154;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_P250)
					{
						*pWeapon->FallbackPaintKit() = 258;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_SCAR20)
					{
						*pWeapon->FallbackPaintKit() = 165;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_G3SG1)
					{
						*pWeapon->FallbackPaintKit() = 438;
						*pWeapon->FallbackWear() = 0.00000001;
					}
					else if (cClass->m_ClassID == (int)CSGOClassID::CKnife)
					{
						// Set whatever knife we want
						switch (Menu::Window.MiscTab.SkinKnife.GetIndex())
						{
						case 0:
							break;
						case 1:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_BAYONET;
							break;
						case 2:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_BUTTERFLY;
							break;
						case 3:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_FLIP;
							break;
						case 4:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_KNIFEGG;
							break;
						case 5:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_GUT;
							break;
						case 6:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_KARAMBIT;
							
							break;
						case 7:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_M9;
							break;
						case 8:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_HUNTSMAN;
							break;
						case 9:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_FALCHION;
							break;
						case 10:
							*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = WEAPON_DAGGER;
							break;
						}

						
					}

				}
			}


			// Netvar proxy hook shit
			ClientClass *pClass = Interfaces::Client->GetAllClasses();
			bool Done = false;
			while (pClass && Done == false)
			{
				const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
				if (!strcmp(pszName, "DT_BaseViewModel"))
				{
					for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
					{
						RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
						const char *name = pProp->m_pVarName;

						// Knives
						if (!strcmp(name, "m_nModelIndex"))
						{
							pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
							Done = true;
							break;
						}
					}
				}
				pClass = pClass->m_pNext;
			}
		}
	}
}

// Paint Traverse Hooked function
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;

	if (!FoundPanel)
	{
		PCHAR szPanelName = (PCHAR)Interfaces::Panels->GetName(vguiPanel);
		if (strstr(szPanelName, "MatSystemTopPanel"))
		{
			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}
	}
	else if (FocusOverlayPanel == vguiPanel)
	{
		if (!Interfaces::Engine->IsInGame())
		{ 
		RECT screen = Render::GetViewport();
		Render::Clear(0, 0, screen.right, 24, Color(28, 136, 0, 200));
		RECT text = Render::GetTextSize(Render::Fonts::MenuWindowTitle, "Ayyware 2.0");
		Render::Text((screen.right/2)-(text.right/2)+1, 2, Color(25, 25, 25,100), Render::Fonts::MenuWindowTitle, "Ayyware 2.0");
		}
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
		{
			Hacks::DrawHacks();
		}	

		Menu::DoFrame();
	}
}

// InPrediction Hooked Function
bool __stdcall Hooked_InPrediction()
{
	IClientEntity* pLocalEntity = NULL;
	float* m_LocalViewAngles = NULL;

	__asm
	{
		MOV pLocalEntity, ESI
		MOV m_LocalViewAngles, EBX
	}

	bool result;
	static InPrediction_ origFunc = (InPrediction_)Hooks::VMTPrediction.GetOriginalFunction(Offsets::VMT::Predict_InPrediction);
	static DWORD *ecxVal = (DWORD*)Interfaces::Prediction;
	DWORD dwCalcPlayerView = Offsets::SigScans::dwCalcPlayerView;
	result = origFunc(ecxVal);

	// If we are in the right place where the player view is calculated
	// Calculate the change in the view and get rid of it
	if ((DWORD)(_ReturnAddress()) == dwCalcPlayerView && Menu::Window.VisualsTab.OtherNoVisualRecoil.GetState())
	{
		Vector viewPunch = pLocalEntity->localPlayerExclusive()->GetViewPunchAngle();
		Vector aimPunch = pLocalEntity->localPlayerExclusive()->GetAimPunchAngle();

		m_LocalViewAngles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		m_LocalViewAngles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		m_LocalViewAngles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		return true;
	}

	return result;
}

void __stdcall Hooked_RunCommand(IClientEntity* pPlayer, CUserCmd* pCmd, IMoveHelper* pMoveHelper)
{
	((RunCommand_)Hooks::VMTPrediction.GetOriginalFunction(19))(Interfaces::Prediction, pPlayer, pCmd, pMoveHelper);

	Interfaces::MoveHelper = pMoveHelper;			
}

BYTE bMoveData[0x200];

void Prediction(CUserCmd* pCmd, IClientEntity* LocalPlayer)
{
	if (Interfaces::MoveHelper && Menu::Window.MiscTab.OtherEnginePrediction.GetState() && LocalPlayer->IsAlive())
	{
		float curtime = Interfaces::Globals->curtime;
		float frametime = Interfaces::Globals->frametime;
		int iFlags = LocalPlayer->GetFlags();

		Interfaces::Globals->curtime = (float)LocalPlayer->GetTickBase() * Interfaces::Globals->interval_per_tick;
		Interfaces::Globals->frametime = Interfaces::Globals->interval_per_tick;

		Interfaces::MoveHelper->SetHost(LocalPlayer);
		
		Interfaces::Prediction->SetupMove(LocalPlayer, pCmd, nullptr, bMoveData);
		Interfaces::GameMovement->ProcessMovement(LocalPlayer, bMoveData);
		Interfaces::Prediction->FinishMove(LocalPlayer, pCmd, bMoveData);
		
		Interfaces::MoveHelper->SetHost(0);

		Interfaces::Globals->curtime = curtime;
		Interfaces::Globals->frametime = frametime;
		*LocalPlayer->GetPointerFlags() = iFlags;
	}
}

// ClientMode CreateMove
bool __fastcall CreateMoveClient_Hooked(void* self, int edx, float frametime, CUserCmd* pCmd)
{
	bool *sendPacket;
	void *basePtr, *inputBaseptr, *clientBaseptr;
	__asm mov basePtr, ebp;
	inputBaseptr = (void *)basePtr;
	sendPacket = *(bool **)inputBaseptr - 0x1C; //Credits : ele0t
	// Backup for safety
	Vector origView = pCmd->viewangles;
	Vector viewforward, viewright, viewup, aimforward, aimright, aimup;
	Vector qAimAngles;
	qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
	AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);

	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal)
	{
		if(pLocal->IsAlive())
		{
			Prediction(pCmd, pLocal);
			Hacks::MoveHacks(pCmd);
		}
		
		ChatSpammer::DoSpamTick();
		ChatSpammer::DoNameSpamTick();
	}

	qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
	AngleVectors(qAimAngles, &aimforward, &aimright, &aimup);
	Vector vForwardNorm;		Normalize(viewforward, vForwardNorm);
	Vector vRightNorm;			Normalize(viewright, vRightNorm);
	Vector vUpNorm;				Normalize(viewup, vUpNorm);

	// Original shit for movement correction
	float forward = pCmd->forwardmove;
	float right = pCmd->sidemove;
	float up = pCmd->upmove;
	if (forward > 450) forward = 450;
	if (right > 450) right = 450;
	if (up > 450) up = 450;
	if (forward < -450) forward = -450;
	if (right < -450) right = -450;
	if (up < -450) up = -450;
	pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
	pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
	pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);

	// -------------          Anti Untrusted        ---------------- //
	if (Menu::Window.HvHTab.AntiUntrusted.GetState())
	{
		NormaliseViewAngle(pCmd->viewangles);

		if (pCmd->viewangles.z != 0.0f)
		{
			pCmd->viewangles.z = 0.00;
		}

		if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
		{
			NormaliseViewAngle(pCmd->viewangles);
			Beep(750, 800);
			if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
			{
				pCmd->viewangles = origView;
				pCmd->sidemove = right;
				pCmd->forwardmove = forward;
			}
		}
	}
	
	return false;
}

// DrawModelExec for chams and shit
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	Color color;
	float flColor[3] = { 0.f };
	static IMaterial* CoveredLit = NULL;
	static IMaterial* OpenLit = NULL;
	static IMaterial* CoveredFlat = NULL;
	static IMaterial* OpenFlat = NULL;

	if(CoveredLit==NULL)
		CoveredLit = CreateMaterial(true);

	if (OpenLit == NULL)
		OpenLit = CreateMaterial(false);

	if (CoveredFlat == NULL)
		CoveredFlat = CreateMaterial(true, false);

	if (OpenFlat == NULL)
		OpenFlat = CreateMaterial(false, false);

	bool DontDraw = false;

	const char* ModelName = Interfaces::ModelInfo->GetModelName((model_t*)pInfo.pModel);
	IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	// Player Chams
	int ChamsStyle = Menu::Window.VisualsTab.cboChams.GetIndex();

	if (ChamsStyle != 0 && Menu::Window.VisualsTab.FiltersPlayers.GetState() && strstr(ModelName, "models/player"))
	{
		if (pLocal && (!Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() ||
			pModelEntity->GetTeamNum() != pLocal->GetTeamNum()))
		{
			IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
			IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;

			if (pModelEntity)
			{
				if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0)
				{
					float alpha = 1.f;

					if (pModelEntity->HasGunGameImmunity())
						alpha = 0.5f;

					if (pModelEntity->GetTeamNum() == 2)
					{
						flColor[0] = 240.f / 255.f;
						flColor[1] = 30.f / 255.f;
						flColor[2] = 35.f / 255.f;
					}
					else
					{
						flColor[0] = 63.f / 255.f;
						flColor[1] = 72.f / 255.f;
						flColor[2] = 205.f / 255.f;
					}

					Interfaces::RenderView->SetColorModulation(flColor);
					Interfaces::RenderView->SetBlend(alpha);
					Interfaces::ModelRender->ForcedMaterialOverride(covered);
					oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

					if (pModelEntity->GetTeamNum() == 2)
					{
						flColor[0] = 247.f / 255.f;
						flColor[1] = 180.f / 255.f;
						flColor[2] = 20.f / 255.f;
					}
					else
					{
						flColor[0] = 32.f / 255.f;
						flColor[1] = 180.f / 255.f;
						flColor[2] = 57.f / 255.f;
					}

					Interfaces::RenderView->SetColorModulation(flColor);
					Interfaces::RenderView->SetBlend(alpha);
					Interfaces::ModelRender->ForcedMaterialOverride(open);
					
				}
				else
				{
					color.SetColor(255, 255, 255, 255);
					ForceMaterial(color, open);
				}
			}
		}
	}

	if (!DontDraw)
		oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	Interfaces::ModelRender->ForcedMaterialOverride(NULL);
}