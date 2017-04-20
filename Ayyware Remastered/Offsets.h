/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Common.h"

namespace Offsets
{
	namespace VMT
	{
		enum  BaseClient
		{
			CHL_GetAllClasses = 8,
			CHL_FrameStageNotify = 36
		};

		enum EngineClient
		{
			Engine_GetScreenSize = 5,
			Engine_GetPlayerInfo = 8,
			Engine_GetLocalPlayer = 12,
			Engine_Time = 14,
			Engine_GetViewAngles = 18,
			Engine_SetViewAngles,
			Engine_GetMaxClients,
			Engine_IsInGame = 26,
			Engine_IsConnected,
			Engine_WorldToScreenMatrix = 37,
			Engine_GetNetChannelInfo = 78,
			Engine_ClientCmd_Unrestricted = 114
		};

		enum Surface
		{
			Surface_DrawSetColorA = 14,
			Surface_DrawSetColorB,
			Surface_DrawFilledRect,
			Surface_DrawOutlinedRect = 18,
			Surface_DrawLine,
			Surface_DrawPolyLine,
			Surface_DrawSetTextFont = 23,
			Surface_DrawSetTextColorCol = 24,
			Surface_DrawSetTextColorRGB,
			Surface_DrawSetTextPos = 26,
			Surface_DrawPrintText = 28,
			Surface_DrawSetTextureRGBA = 37,
			Surface_DrawSetTexture,
			Surface_CreateNewTextureID = 43,
			Surface_SurfaceGetCursorPos = 66,
			Surface_FontCreate = 71,
			Surface_SetFontGlyphSet,
			Surface_GetTextSize = 79,
			Surface_DrawOutlinedCircle = 103,
			Surface_DrawTexturedPolygon = 106
		};

		enum  Panel
		{
			Panel_GetName = 36,
			Panel_PaintTraverse = 41
		};

		enum Prediction
		{
			Predict_InPrediction = 14
		};

		enum ModelRender
		{
			ModelRender_ForcedMaterialOverride = 1,
			ModelRender_DrawModelExecute = 21
		};

		enum IMaterial
		{
			Material_GetName = 0,
			Material_SetMaterialVarFlag = 30,
			Material_GetMaterialVarFlag = 31,
			Material_AlphaModulate = 28,
			Material_ColorModulate = 29,
			Material_IncrementReferenceCount = 14
		};

		enum MaterialSystem
		{
			MaterialSystem_FindMaterial = 84,
			MaterialSystem_CreateMaterial = 83
		};


	};

	namespace SigScans
	{
		extern DWORD dwCalcPlayerView;
		extern DWORD dwSpreadVmt;
		extern DWORD dwGetCSWpnData;
		extern DWORD dwKeyValues_KeyValues;
		extern DWORD dwKeyValues_LoadFromBuffer;
	};

	void Init();
};