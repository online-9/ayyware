/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "RageBot.h"
#include "Menu.h"
#include "Ayywall.h"
#include "MD5.h"

void CRageBot::Setup()
{
	IsLocked = false;
	TargetID = -1;
	Utilities::Log("Rage Setup");
}

void CRageBot::Draw()
{

}

void CRageBot::Move(CUserCmd *pCmd)
{
	// Aimbot
	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd);
	
	// Recoil
	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	// No Spread
	if (Menu::Window.HvHTab.NoSpread.GetState())
		DoNoSpread(pCmd);

	// Anti-Aim
	if (Menu::Window.HvHTab.AntiAimEnable.GetState())
		DoAntiAim(pCmd);
	

	//if (Menu::Window.HvHTab.FakeLag.GetState())
		//DoFakelag(pCmd, bSendPacket);

	LastAngles = pCmd->viewangles;
}
template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}
float LagFix()
{
	float updaterate = Interfaces::CVar->FindVar("cl_updaterate")->fValue;
	ConVar* minupdate = Interfaces::CVar->FindVar("sv_minupdaterate");
	ConVar* maxupdate = Interfaces::CVar->FindVar("sv_maxupdaterate");

	if (minupdate && maxupdate)
		updaterate = maxupdate->fValue;

	float ratio = Interfaces::CVar->FindVar("cl_interp_ratio")->fValue;

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = Interfaces::CVar->FindVar("cl_interp")->fValue;
	ConVar* cmin = Interfaces::CVar->FindVar("sv_client_min_interp_ratio");
	ConVar* cmax = Interfaces::CVar->FindVar("sv_client_max_interp_ratio");

	if (cmin && cmax && cmin->fValue != 1)
		ratio = clamp(ratio, cmin->fValue, cmax->fValue);


	return max(lerp, ratio / updaterate);
}
#define TIME_TO_TICKS( dt )	( ( int )( 0.5f + ( float )( dt ) / Interfaces::Globals->interval_per_tick ) )
// Functionality
void CRageBot::DoAimbot(CUserCmd *pCmd)
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = HackManager.pLocal();
	bool FindNewTarget = true;
	//IsLocked = false;

	// Don't aimbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (pWeapon)
	{

		if (pWeapon->GetAmmoInClip() == 0 || !IsBallisticWeapon(pWeapon))
		{

			//TargetID = 0;
			//pTarget = nullptr;
			//HitBox = -1;
			return;
		}
	}
	else
		return;


	// Make sure we have a good target
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View; Interfaces::Engine->GetViewAngles(View);
				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())
					FindNewTarget = false;
			}
		}
	}


	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		// Target selection type
		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex())
		{
		case 0:
			TargetID = GetTargetCrosshair();
			break;
		case 1:
			TargetID = GetTargetDistance();
			break;
		case 2:
			TargetID = GetTargetHealth();
			break;
		}

		// Memes
		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
			IsLocked = false;
		}
	}


	// If we finally have a good target
	if (TargetID >= 0 && pTarget)
	{
		HitBox = HitScan(pTarget);

		// Key
		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				IsLocked = false;
				return;
			}
		}

		Vector AimPoint = GetHitboxPosition(pTarget, HitBox);

		pWeapon->UpdateAccuracyPenalty();
		float Spread = NULL;

		float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		float NextAttack = pWeapon->GetNextPrimaryAttack();

		if (!Menu::Window.RageBotTab.AccuracySpreadLimit.GetState() || Spread <= (Menu::Window.RageBotTab.AccuracyMinimumSpread.GetValue() / 100.f))
		{
			if (AimAtPoint(pLocal, AimPoint, pCmd) && ( (NextAttack <= flServerTime) || !Menu::Window.RageBotTab.AimbotPSilent.GetState()))
			{
				//IsLocked = true;
				if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
				{
					
					
					pCmd->buttons |= IN_ATTACK;
					
				}
			}
		}

		if (Menu::Window.RageBotTab.AccuracyAutoStop.GetState())
		{
			pCmd->forwardmove = 0.f;
			pCmd->sidemove = 0.f;
		}

		if (Menu::Window.RageBotTab.AccuracyAutoCrouch.GetState())
		{
			pCmd->buttons |= IN_DUCK;
		}

	}

	// Auto Pistol
	static bool WasFiring = false;
	CSWeaponInfo* WeaponInfo = pWeapon->GetCSWpnData();
	if (!WeaponInfo->m_IsFullAuto && Menu::Window.RageBotTab.AimbotAutoPistol.GetState())
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
		}

		WasFiring = pCmd->buttons & IN_ATTACK ? true : false;
	}
	pCmd->tick_count = TIME_TO_TICKS(LagFix());
}

bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntity)
{
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != HackManager.pLocal()->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntity->GetTeamNum() != HackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetIndex() == 1)
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					// Player List
					if(!IsFriendly(pEntity->GetIndex()))
						return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}

float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(View, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);

	// Get delta vector between our local eye position and passed vector
	Delta = AimPos - Origin;
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (180.f / PI));
}

int CRageBot::GetTargetCrosshair()
{
	// Target selection
	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = HackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov < minFoV || IsAimPrio(i))
				{
					minFoV = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetDistance()
{
	// Target selection
	int target = -1;
	int minDist = 99999;

	IClientEntity* pLocal = HackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if ((Distance < minDist || IsAimPrio(i)) && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetHealth()
{
	// Target selection
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = HackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::HitScan(IClientEntity* pEntity)
{
	std::vector<int> HitBoxesToScan;
	//bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();

	// Get the hitboxes to scan
#pragma region GetHitboxesToScan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	if (HitScanMode == 0)
	{
		// No Hitscan, just a single hitbox
		switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex())
		{
		case 0:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			break;
		case 1:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			break;
		case 2:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			break;
		case 3:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			break;
		}
	}
	else
	{
		switch (HitScanMode)
		{
		case 1:
			// head/body
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			break;
		case 2:
			// basic +(arms, thighs)
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
		case 3:
			// heaps ++(just all the random shit)
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
		}
	}
#pragma endregion Get the list of shit to scan

	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	IClientEntity* pLocal = HackManager.pLocal();
	Vector Start = pLocal->GetViewOffset() + pLocal->GetOrigin();
	CSWeaponInfo* weapInfo = ((CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle()))->GetCSWpnData();
	// check hits
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (AWall)
		{
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			
			if (bIsPointPenetrable(weapInfo, Start, Point))
			{
				return HitBoxID;
			}
		}
		else
		{
			if (IsVisible(HackManager.pLocal(), pEntity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

void reen(Vector& vec)
{
	for (int i = 0; i < 3; ++i)
	{
		while (vec[i] > 180.f)
			vec[i] -= 360.f;

		while (vec[i] < -180.f)
			vec[i] += 360.f;
	}

	vec[2] = 0.f;
}

void reec(Vector& vecAngles)
{
	if (vecAngles[0] > 89.f)
		vecAngles[0] = 89.f;
	if (vecAngles[0] < -89.f)
		vecAngles[0] = -89.f;
	if (vecAngles[1] > 180.f)
		vecAngles[1] = 180.f;
	if (vecAngles[1] < -180.f)
		vecAngles[1] = -180.f;

	vecAngles[2] = 0.f;
}

float RandomFloat(float min, float max)
{
	typedef float(*RandomFloat_t)(float, float);
	static RandomFloat_t m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat");
	return m_RandomFloat(min, max);
}

void RandomSeed(UINT seed)
{
	typedef void(*RandomSeed_t)(UINT);
	static RandomSeed_t m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed");
	m_RandomSeed(seed);
	return;
}

void GetSpreadAngle(UINT iSeed, CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	Vector vecForward, vecRight, vecDir, vecUp, vecAntiDir;
	float flSpread, flInaccuracy, flSpreadX, flSpreadY;

	Vector qAntiSpread;
	reen(pCmd->viewangles);
	reec(pCmd->viewangles);

	pWeapon->UpdateAccuracyPenalty();

	flSpread = pWeapon->GetSpread();

	flInaccuracy = pWeapon->GetInaccuracy();
	pCmd->random_seed = MD5_PseudoRandom(pCmd->command_number) & 0x7FFFFFFF;
	RandomSeed((pCmd->random_seed & 0xFF) + 1);


	float flRandPi_1 = RandomFloat(0.0f, 2.0f * PI);
	float flRandInaccuracy = RandomFloat(0.0f, flInaccuracy);
	float flRandPi_2 = RandomFloat(0.0f, 2.0f * PI);
	float flRandSpread = RandomFloat(0, flSpread);

	flSpreadX = cos(flRandPi_1) * flRandInaccuracy + cos(flRandPi_2) * flRandSpread;
	flSpreadY = sin(flRandPi_1) * flRandInaccuracy + sin(flRandPi_2) * flRandSpread;

	AngleVectors(pCmd->viewangles, &vecForward, &vecRight, &vecUp);

	vecDir.x = (float)((float)(vecRight.x * flSpreadX) + vecForward.x) + (float)(vecUp.x * flSpreadY);
	vecDir.y = (float)((float)(flSpreadX * vecRight.y) + vecForward.y) + (float)(flSpreadY * vecUp.y);
	vecDir.z = (float)((float)(vecRight.z * flSpreadX) + vecForward.z) + (float)(vecUp.z * flSpreadY);

	vecAntiDir = vecForward + (vecRight * -flSpreadX) + (vecUp * -flSpreadY);

	vecAntiDir.NormalizeInPlace();

	VectorAngles(vecAntiDir, qAntiSpread);

	pCmd->viewangles = qAntiSpread;
	reen(pCmd->viewangles);
	reec(pCmd->viewangles);
}

void CRageBot::DoNoSpread(CUserCmd *pCmd)
{
	IClientEntity* pLocal = HackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (pWeapon)
		GetSpreadAngle(pCmd->random_seed, pCmd, pWeapon);
}
/*
void CRageBot::DoFakelag(CUserCmd *pCmd, bool* bSendPacket)
{
	bool WasShooting = false;
	int iChoke = 13;
	static int iFakeLag = 0;

	if (!WasShooting)
	{
		if (iFakeLag <= iChoke && iFakeLag > 0)
		{
			*bSendPacket = false;
		}
		else
		{
			*bSendPacket = true;
			iFakeLag = 0;
		}
	}
	else
	{
		if (bSendPacket)
			WasShooting = false;
		else
			WasShooting = true;
	}

	iFakeLag++;
}
*/
void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	// Ghetto rcs shit, implement properly later
	IClientEntity* pLocal = HackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

float FovToPoint(Vector ViewOffSet, Vector View, Vector Point)
{
	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(View, &Forward);
	Vector AimPos = Point;

	// Get delta vector between our local eye position and passed vector
	Delta = AimPos - Origin;
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (180.f / PI));
}

bool CRageBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd)
{
	// Return value determines if the aimbot should fire the weapon
	bool ReturnValue = false;

	// Make sure it isn't trying to aim at 0,0,0 because it's probably a bug
	if (point.Length() == 0) return ReturnValue;

	// Work out our aim angles bla bla bla
	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	NormaliseViewAngle(angles);
	
	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return ReturnValue;
	}

	// WE GOOD, WE GUNNA AIM AT THIS BITCH
	IsLocked = true;

	// Apply AimStep if needed
	int AimStepValue = Menu::Window.RageBotTab.AimbotAimstep.GetIndex();
	if (AimStepValue != 0)
	{
		Vector delta = angles - LastAngles;
		float dist = AimStepValue == 1 ? 10.f : 5.f;
		NormaliseViewAngle(delta);
		if (FovToPoint(HackManager.pLocal()->GetViewOffset() + HackManager.pLocal()->GetOrigin(), LastAngles, point) > dist)
		{
			Normalize(delta, delta);
			delta *= dist;
			angles = LastAngles + delta;
			
		}
		else
		{
			ReturnValue = true;
		}
	}
	else
		ReturnValue = true;


	NormaliseViewAngle(angles);

	// Apply the angles to cmd and maybe the client
	pCmd->viewangles = angles;
	if (!Menu::Window.RageBotTab.AimbotSilentAim.GetState())
		Interfaces::Engine->SetViewAngles(angles);

	return ReturnValue;
}

namespace AntiAims
{
	// Pitches
	void StaticPitch(CUserCmd *pCmd, bool up)
	{
		if (up)
			// Up
			pCmd->viewangles.x = -88;
		else
			// Down
			pCmd->viewangles.x = 88;
	}

	void Emotion(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 70;
	}

	void JitterPitch(CUserCmd *pCmd)
	{
		static bool up = true;
		if (up) pCmd->viewangles.x = -88;
		else pCmd->viewangles.x = 88;
		up = !up;
	}

	void JitterPitch2(CUserCmd *pCmd)
	{
		static bool up = true;
		if (up) pCmd->viewangles.x = 70;
		else pCmd->viewangles.x = 88;
		up = !up;
	}

	void FakeDown(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = -179.9995f;
	}

	// Yaws
	void FastSpin(CUserCmd *pCmd)
	{
		static int y2 = -179;
		int spinBotSpeedFast = 100;

		y2 += spinBotSpeedFast;

		if (y2 >= 179)
			y2 = -179;

		pCmd->viewangles.y = y2;
	}

	void SlowSpin(CUserCmd *pCmd)
	{
		static int y = -179;
		int spinBotSpeedSlow = 15;

		y += spinBotSpeedSlow;

		if (y >= 179)
			y = -179;

		pCmd->viewangles.y = y;

	}

	void BackJitter(CUserCmd *pCmd)
	{
		int random = rand() % 100;

		// Small chance of starting fowards
		if (random < 98)
			// Look backwards
			pCmd->viewangles.y -= 180;

		// Some jitter
		if (random < 15)
		{
			float change = -70 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -90 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}

	}

	void Jitter2(CUserCmd *pCmd)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 90;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y -= 90;
		}

		int re = rand() % 4 + 1;


		if (jitterangle <= 1)
		{
			if (re == 4)
				pCmd->viewangles.y += 180;
			jitterangle += 1;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			if (re == 4)
				pCmd->viewangles.y -= 180;
			jitterangle += 1;
		}
		else
		{
			jitterangle = 0;
		}


	}

	void Flip(CUserCmd *pCmd)
	{
		static bool back = false;
		back = !back;
		if (back)
			pCmd->viewangles.y -= 180;
	}

	bool EdgeAntiAim(IClientEntity* pLocalBaseEntity, CUserCmd* cmd, float flWall, float flCornor)
	{
		Ray_t ray;
		trace_t tr;

		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocalBaseEntity;

		auto bRetVal = false;
		auto vecCurPos = pLocalBaseEntity->GetOrigin() + pLocalBaseEntity->GetViewOffset();

		for (float i = 0; i < 360; i++)
		{
			Vector vecDummy(10.f, cmd->viewangles.y, 0.f);
			vecDummy.y += i;

			vecDummy.NormalizeInPlace();

			Vector vecForward;
			AngleVectors(vecDummy, &vecForward);

			auto flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
			vecForward *= flLength;

			ray.Init(vecCurPos, (vecCurPos + vecForward));
			Interfaces::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

			if (tr.fraction != 1.0f)
			{
				Vector qAngles;
				auto vecNegate = tr.plane.normal;

				vecNegate *= -1.f;
				VectorAngles(vecNegate, qAngles);

				vecDummy.y = qAngles.y;

				vecDummy.NormalizeInPlace();
				trace_t leftTrace, rightTrace;

				Vector vecLeft;
				AngleVectors(vecDummy + Vector(0.f, 30.f, 0.f), &vecLeft);

				Vector vecRight;
				AngleVectors(vecDummy - Vector(0.f, 30.f, 0.f), &vecRight);

				vecLeft *= (flLength + (flLength * sin(DEG2RAD(30.f))));
				vecRight *= (flLength + (flLength * sin(DEG2RAD(30.f))));

				ray.Init(vecCurPos, (vecCurPos + vecLeft));
				Interfaces::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &leftTrace);

				ray.Init(vecCurPos, (vecCurPos + vecRight));
				Interfaces::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &rightTrace);

				if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
					vecDummy.y -= flCornor; // left
				else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
					vecDummy.y += flCornor; // right           

				cmd->viewangles.y = vecDummy.y;
				cmd->viewangles.y -= flWall;
				bRetVal = true;
			}
		}

		return bRetVal;
	}
}

// AntiAim
void CRageBot::DoAntiAim(CUserCmd *pCmd)
{
	// If the aimbot is doing something don't do anything
	if (pCmd->buttons & IN_ATTACK || (Menu::Window.RageBotTab.AimbotAimstep.GetIndex() > 0 && IsLocked))
		return;

	// Weapon shit
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(HackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
		// Knives or grenades
		if (!IsBallisticWeapon(pWeapon))
			return;
	}

	// Anti-Aim Pitch
	switch (Menu::Window.HvHTab.AntiAimPitch.GetIndex())
	{
	case 0:
		// No Pitch AA
		break;
	case 1:
		// up
		AntiAims::StaticPitch(pCmd, true);
		break;
	case 2:
		// Down
		AntiAims::StaticPitch(pCmd, false);
		break;
	case 3:
		// Emotion
		AntiAims::Emotion(pCmd);
		break;
	case 4:
		// Jitter
		AntiAims::JitterPitch(pCmd);
		break;
	case 5:
		// Jitter 2
		AntiAims::JitterPitch2(pCmd);
		break;
	case 6:
		// FakeDown
		AntiAims::FakeDown(pCmd);
		break;
	}

	//Anti-Aim Yaw
	switch (Menu::Window.HvHTab.AntiAimYaw.GetIndex())
	{
	case 0:
		// No Yaw AA
		break;
	case 1:
		// Fast Spin
		AntiAims::FastSpin(pCmd);
		break;
	case 2:
		// Slow Spin
		AntiAims::SlowSpin(pCmd);
		break;
	case 3:
		// Inverse
		pCmd->viewangles.y -= 180;
		break;
	case 4:
		// Jitter
		AntiAims::BackJitter(pCmd);
		break;
	case 5:
		// Jitter 2
		AntiAims::Jitter2(pCmd);
		break;
	case 6:
		// Flip
		AntiAims::Flip(pCmd);
		break;
	case 7:
		// Edge
		AntiAims::EdgeAntiAim(HackManager.pLocal(), pCmd, 0, 89.f);
		break;
	};
}