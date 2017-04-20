/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Valve\ClientRecvProps.h"
#include "VFuncs.h"
#include "Offsets.h"

class IBaseClientDLL
{
public:

	ClientClass* GetAllClasses(void)
	{
		typedef ClientClass* (__thiscall* OriginalFn)(PVOID);
		return call_vfunc<OriginalFn>(this, Offsets::VMT::CHL_GetAllClasses)(this);
	}

};