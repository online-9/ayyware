//
// Syn's "Storm" Cheat
// Started October 2015
//

// Source engine interface management

#include "Interfaces.h"
#include "Utilities.h"

//#define IFACEDUMP

Interfaces::InterfaceManager::InterfaceManager()
{
	ModuleBase = 0;
	pIntHead = 0;
}

Interfaces::InterfaceManager::InterfaceManager(std::string strModule)
{
	ModuleBase = 0;
	pIntHead = 0;
	Setup(strModule);
}

void Interfaces::InterfaceManager::Setup(std::string strName)
{
	strModuleName = strName;
	ModuleBase = (HMODULE)Utilities::Memory::WaitOnModuleHandle(strName.c_str());

	if (ModuleBase)
	{
		DWORD* pCreateInterfaceFn = (DWORD*)GetProcAddress(ModuleBase, "CreateInterface");

		// Step 1: Find the jump to the main block of code
		pCreateInterfaceFn = (DWORD*)((DWORD)pCreateInterfaceFn + 0x4); // Later on replace this +4 with a search for the first jmp(0xE9)
		int JmpConv = *(int*)((DWORD)pCreateInterfaceFn + 1); // + 1 to skip the opcode		
		pCreateInterfaceFn = (DWORD*)(5 + (DWORD)pCreateInterfaceFn + (DWORD)JmpConv); // Jmp is relative to the end of this line (hence the +5 bytes)

		// Step 2: Get the head of the list
		// MOV ESI,DWORD PTR DS:[1A69F904]
		pIntHead = *(InterfacesHead**)((DWORD)pCreateInterfaceFn + 0x6); // Later use a search instead
	}
}

void* Interfaces::InterfaceManager::GetInterface(std::string strName)
{
	if (pIntHead)
	{
		DWORD pInt = NULL;
		InterfaceNode* pNode = pIntHead->HeadNode;

		// Search for the interface
		while (pNode)
		{
			std::string nodeName = pNode->pName;
			if (nodeName.find(strName) != std::string::npos)
			{
				// If we find it, call their function so they return us a pointer
				DWORD dest = (DWORD)pNode->fncGet;
				__asm
				{
					call dest
					mov pInt, eax
				}
				Utilities::SetConsoleColor(FOREGROUND_INTENSE_GREEN);
				printf("[Interfaces]");
				Utilities::SetConsoleColor(FOREGROUND_WHITE);
				printf(" %s => 0x%x\n", pNode->pName, pInt);
				return (void*)pInt;
			}
			else
			{
				// It's not them, try the next one
				pNode = pNode->pNext;
			}
		}
	}

	// We didn't find it
	return nullptr;
}

void Interfaces::InterfaceManager::DumpAllInterfaces()
{
	printf("%-16s%-44s%10s\n", "Module", "Interface", "Instance");
	printf("----------------------------------------------------------------------\n");
	if (pIntHead)
	{
		DWORD pInt = NULL;
		InterfaceNode* pNode = pIntHead->HeadNode;

		// Loop through them all
		while (pNode)
		{

			// Get a pointer to their instance
			DWORD dest = (DWORD)pNode->fncGet;
			__asm
			{
				call dest
				mov pInt, eax
			}

			printf("%-16s%-44s%10x\n", strModuleName.c_str(), pNode->pName, pInt);

			// Move on to the next one
			pNode = pNode->pNext;
		}
	}
	printf("----------------------------------------------------------------------\n");
}

void Interfaces::Initialize()
{
	Interfaces::InterfaceManager ClientMan("client.dll");
	Interfaces::InterfaceManager EngineMan("engine.dll");
	Interfaces::InterfaceManager VGUI2Man("vgui2.dll");
	Interfaces::InterfaceManager MatSurfaceMan("vguimatsurface.dll");
	Interfaces::InterfaceManager MatSystemMan("materialsystem.dll");
	Interfaces::InterfaceManager PhysMan("vphysics.dll");
	Interfaces::InterfaceManager StdLibMan("vstdlib.dll");


#ifdef IFACEDUMP
	ClientMan.DumpAllInterfaces();
	EngineMan.DumpAllInterfaces();
	VGUI2Man.DumpAllInterfaces();
	MatSurfaceMan.DumpAllInterfaces();
	MatSystemMan.DumpAllInterfaces();
	PhysMan.DumpAllInterfaces();
	StdLibMan.DumpAllInterfaces();
#endif

	// Get your instances, for example
	Client = (IBaseClientDLL*)ClientMan.GetInterface("VClient0");
	Engine = (IVEngineClient*)EngineMan.GetInterface("VEngineClient0"); // without 0 we get the stringtable lol
	Panels = (IPanel*)VGUI2Man.GetInterface("VGUI_Panel");
	EntList = (IClientEntityList*)ClientMan.GetInterface("VClientEntityList");
	Surface = (ISurface*)MatSurfaceMan.GetInterface("VGUI_Surface");
	DebugOverlay = (IVDebugOverlay*)EngineMan.GetInterface("VDebugOverlay");
	Prediction = (IPrediction*)ClientMan.GetInterface("VClientPrediction");
	Trace = (IEngineTrace*)EngineMan.GetInterface("EngineTraceClient");
	PhysProps = (IPhysicsSurfaceProps*)PhysMan.GetInterface("VPhysicsSurfaceProps");
	CVar = (ICVar*)StdLibMan.GetInterface("VEngineCvar");
	MaterialSystem = (CMaterialSystem*)MatSystemMan.GetInterface("VMaterialSystem0");
	RenderView = (CVRenderView*)EngineMan.GetInterface("VEngineRenderView");
	ModelRender = (IVModelRender*)EngineMan.GetInterface("VEngineModel0");
	ModelInfo = (CModelInfo*)EngineMan.GetInterface("VModelInfoClient");
	GameMovement = (IGameMovement*)ClientMan.GetInterface("GameMovement");
	GameEventManager = (IGameEventManager*)EngineMan.GetInterface("GAMEEVENTSMANAGER002");


	// Get ClientMode Pointer
	DWORD* ppClientMode;
	ppClientMode = nullptr; // before "scripts/vgui_screens.txt"
	DWORD p = Utilities::Memory::FindPattern("client.dll", (BYTE*)"\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xA8\x01\x75\x1A\x83\xC8\x01\xA3\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x04\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x56", "xx????????xxxxxxxx????x????x????x????xxxx????x????x");
	if (p)
	{
		ppClientMode = **(DWORD***)(p + 2);
		ClientMode = ppClientMode;
	}

	// Search through the first entry of the Client VTable
	// The initializer contains a pointer to the 'GlobalsVariables' Table
	PDWORD pdwClient = (PDWORD)*(PDWORD)Client;
	DWORD dwInitAddr = (DWORD)(pdwClient[0]);
	for (DWORD dwIter = 0; dwIter <= 0xFF; dwIter++)
	{
		if (*(PBYTE)(dwInitAddr + dwIter - 1) == 0x08 && *(PBYTE)(dwInitAddr + dwIter) == 0xA3)
		{
			Globals = (CGlobalVarsBase*)*(PDWORD)*(PDWORD)(dwInitAddr + dwIter + 1);
			break;
		}
	}

	Utilities::Log("Interface Manager Ready");
}

// Interface instances here, for example
IBaseClientDLL* Interfaces::Client;
IVEngineClient* Interfaces::Engine;
IPanel* Interfaces::Panels;
IClientEntityList* Interfaces::EntList;
ISurface* Interfaces::Surface;
IVDebugOverlay* Interfaces::DebugOverlay;
DWORD *Interfaces::ClientMode;
CGlobalVarsBase *Interfaces::Globals;
IPrediction *Interfaces::Prediction;
CMaterialSystem* Interfaces::MaterialSystem;
CVRenderView* Interfaces::RenderView;
IVModelRender* Interfaces::ModelRender;
CModelInfo* Interfaces::ModelInfo;
IEngineTrace* Interfaces::Trace;
IPhysicsSurfaceProps* Interfaces::PhysProps;
ICVar* Interfaces::CVar;
IMoveHelper* Interfaces::MoveHelper;
IGameMovement* Interfaces::GameMovement;
IGameEventManager *Interfaces::GameEventManager;