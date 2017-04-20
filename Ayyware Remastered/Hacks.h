/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "SDK.h"
#include <list>

namespace Hacks
{
	void Initialize();
	void DrawHacks();
	void MoveHacks(CUserCmd* pCmd);
};

// Hack features implement this shit
class Hack
{
public:
	virtual void Setup() = 0;
	virtual void Draw() = 0;
	virtual void Move(CUserCmd* pCmd) = 0;
};


// Keeps track of implemented hack classes
class CHackManager
{
public:
	void Setup();
	void Draw();
	void Move(CUserCmd* pCmd);

	void AddHack(Hack* pHack);
	inline IClientEntity* pLocal() { return pLocalInstance; };
private:
	std::list<Hack*> HackList;
	IClientEntity* pLocalInstance;
};

// Instance of this shit
extern CHackManager HackManager;