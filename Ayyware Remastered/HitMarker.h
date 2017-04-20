/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Interfaces.h"
#include "Hacks.h"

class cGameEventManager : public IGameEventListener
{
public:
	void FireGameEvent(IGameEvent *event);
	void RegisterSelf();
	bool ShouldHitmarker();
	bool ShouldRoundStart();

private:
	bool doHitmarker;
	bool doRoundStart;
};

class CHitMarker : public Hack
{
public:
	void Setup();
	void Draw();
	void Move(CUserCmd *pCmd) {}
	cGameEventManager EventMan;
};

extern CHitMarker HitMarker;