/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Common.h"
#include "SDK.h"

namespace Interfaces
{
	class InterfacesHead;
	class InterfaceNode;

	class InterfacesHead
	{
	public:
		InterfaceNode* HeadNode; //0x0000 
	};//Size=0x0040

	class InterfaceNode
	{
	public:
		void* fncGet; //0x0000 
		char* pName; //0x0004 
		InterfaceNode* pNext; //0x0008 
	};//Size=0x001C

	class InterfaceManager
	{
	public:
		InterfaceManager(std::string strModule);
		InterfaceManager();

		void Setup(std::string strModule);
		void* GetInterface(std::string strName);
		void DumpAllInterfaces();

	private:
		std::string strModuleName;
		HMODULE ModuleBase;
		InterfacesHead* pIntHead;
	};

	//----------------------------------------------------------------------------

	void Initialize();

	// Interface instances here, for example
	extern IBaseClientDLL* Client;
	extern IVEngineClient* Engine;
	extern IPanel* Panels;
	extern IClientEntityList* EntList;
	extern ISurface* Surface;
	extern IVDebugOverlay* DebugOverlay;
	extern DWORD *ClientMode;
	extern CGlobalVarsBase *Globals;
	extern IPrediction *Prediction;
	extern CMaterialSystem* MaterialSystem;
	extern CVRenderView* RenderView;
	extern IVModelRender* ModelRender;
	extern CModelInfo* ModelInfo;
	extern IEngineTrace* Trace;
	extern IPhysicsSurfaceProps* PhysProps;
	extern ICVar *CVar;
	extern IMoveHelper *MoveHelper;
	extern IGameMovement *GameMovement;
	extern IGameEventManager *GameEventManager;

};
