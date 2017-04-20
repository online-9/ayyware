/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Hacks.h"

class CRageBot : public Hack
{
public:
	void Setup();
	void Draw();
	void Move(CUserCmd *pCmd);

	int GetTargetCrosshair();
	int GetTargetDistance();
	int GetTargetHealth();
	bool TargetMeetsRequirements(IClientEntity* pEntity);
	float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int HitBox);
	int HitScan(IClientEntity* pEntity);
	bool AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd);

	// Functionality
	void DoAimbot(CUserCmd *pCmd);
	void DoNoSpread(CUserCmd *pCmd);
	void DoNoRecoil(CUserCmd *pCmd);
	void DoFakelag(CUserCmd *pCmd, bool* bSendPacket);
	// AntiAim
	void DoAntiAim(CUserCmd *pCmd);

	// Aimbot
	bool IsLocked;
	int TargetID;
	int HitBox;
	Vector AimPoint;
	
	Vector LastAngles;
};