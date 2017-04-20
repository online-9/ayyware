/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "VFuncs.h"
#include "Offsets.h"
#include "Valve\MiscDefinitions.h"

class INetChannelInfo;


// EngineClient
class IVEngineClient
{
public:
	void GetScreenSize(int& width, int& height)
	{
		typedef void(__thiscall* oGetScreenSize)(PVOID, int&, int&);
		return call_vfunc< oGetScreenSize >(this, Offsets::VMT::Engine_GetScreenSize)(this, width, height);
	}
	bool GetPlayerInfo(int ent_num, player_info_t *pinfo)
	{
		typedef bool(__thiscall* oGetPlayerInfo)(PVOID, int, player_info_t*);
		return call_vfunc< oGetPlayerInfo >(this, Offsets::VMT::Engine_GetPlayerInfo)(this, ent_num, pinfo);
	}
	int GetLocalPlayer()
	{
		typedef int(__thiscall* oLocal)(PVOID);
		return call_vfunc< oLocal >(this, Offsets::VMT::Engine_GetLocalPlayer)(this);
	}
	int GetPlayerForUserID(int userID)
	{
		typedef int(__thiscall* oGetPlayerForUserID)(PVOID,int);
		return call_vfunc< oGetPlayerForUserID >(this, (Offsets::VMT::Engine_GetPlayerInfo+1))(this, userID);
	}
	float Time()
	{
		typedef float(__thiscall* oTime)(PVOID);
		return call_vfunc< oTime >(this, Offsets::VMT::Engine_Time)(this);
	}
	void GetViewAngles(Vector& vAngles)
	{
		typedef void(__thiscall* oSetViewAngles)(PVOID, Vector&);
		return call_vfunc< oSetViewAngles >(this, Offsets::VMT::Engine_GetViewAngles)(this, vAngles);
	}
	void SetViewAngles(Vector& vAngles)
	{
		typedef void(__thiscall* oSetViewAngles)(PVOID, Vector&);
		return call_vfunc< oSetViewAngles >(this, Offsets::VMT::Engine_SetViewAngles)(this, vAngles);
	}
	int GetMaxClients()
	{
		typedef bool(__thiscall* oGetMaxClients)(PVOID);
		return call_vfunc< oGetMaxClients >(this, Offsets::VMT::Engine_GetMaxClients)(this);
	}
	bool IsConnected()
	{
		typedef bool(__thiscall* oGetScreenSize)(PVOID);
		return call_vfunc< oGetScreenSize >(this, Offsets::VMT::Engine_IsConnected)(this);
	}
	bool IsInGame()
	{
		typedef bool(__thiscall* oLocal)(PVOID);
		return call_vfunc< oLocal >(this, Offsets::VMT::Engine_IsInGame)(this);
	}
	const matrix3x4& WorldToScreenMatrix()
	{
		typedef const matrix3x4& (__thiscall* oWorldToScreenMatrix)(PVOID);
		return call_vfunc< oWorldToScreenMatrix >(this, Offsets::VMT::Engine_WorldToScreenMatrix)(this);
	}
	void ClientCmd_Unrestricted(char  const* cmd)
	{
		typedef void(__thiscall* oClientCmdUnres)(PVOID, const char*, char);
		return call_vfunc<oClientCmdUnres>(this, Offsets::VMT::Engine_ClientCmd_Unrestricted)(this, cmd, 0);
	}
	INetChannelInfo* GetNetChannelInfo()
	{
		typedef INetChannelInfo*(__thiscall* oLocal)(PVOID);
		return call_vfunc< oLocal >(this, Offsets::VMT::Engine_GetNetChannelInfo)(this);
	}
};

class NET_SetConVar
{
public:
	NET_SetConVar(const char* name, const char* value);
	~NET_SetConVar();
private:
	DWORD pad[13];
};

class INetChannelInfo
{
public:
	void SendNetMsg(NET_SetConVar& convar)
	{
		typedef void(__thiscall* oSendNetMsg)(PVOID, NET_SetConVar&, int, int);
		return call_vfunc<oSendNetMsg>(this, 42)(this, convar, 0, 0);
	}
};