/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "GUI.h"
#include "Controls.h"

struct PlayerListItem_t
{
	bool Friendly;
	bool AimPrio;
	bool Callout;
};

DWORD GetPlayerListIndex(int EntId);
bool IsFriendly(int EntId);
bool IsAimPrio(int EntId);
bool IsCalloutTarget(int EntId);


extern std::map<DWORD, PlayerListItem_t> PlayerList;

class CRageTab : public CTab
{
public:
	void Setup();

	CGroupBox grpAimbot;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CSlider	  AimbotFov;
	CCheckBox AimbotSilentAim;
	CCheckBox AimbotPSilent;
	CCheckBox AimbotAutoPistol;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CComboBox AimbotAimstep;

	CGroupBox grpTarget;
	CComboBox TargetSelection;
	CComboBox TargetFriendlyFire;
	CComboBox TargetHitbox;
	CComboBox TargetHitscan;

	CGroupBox grpAccuracy;
	//CCheckBox AccuracySpread;
	CCheckBox AccuracyRecoil;
	CCheckBox AccuracyAutoWall;
	//CSlider	  AccuracyMinimumDamage;
	CCheckBox AccuracyAutoStop;
	CCheckBox AccuracyAutoCrouch;
	CCheckBox AccuracySpreadLimit;
	CSlider	  AccuracyMinimumSpread;
};

class CLegitTab : public CTab
{
public:
	void Setup();

	// Aimbot Settings
	CGroupBox AimbotGroup;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CCheckBox AimbotAutoPistol;
	CSlider   AimbotInaccuracy;


	// Main
	CGroupBox WeaponMainGroup;
	CSlider   WeaponMainSpeed;
	CSlider   WeaponMainFoV;
	CCheckBox WeaponMainRecoil;
	CComboBox WeaponMainHitbox;

	// Pistol
	CGroupBox WeaponPistGroup;
	CSlider   WeaponPistSpeed;
	CSlider   WeaponPistFoV;
	CCheckBox WeaponPistRecoil;
	CComboBox WeaponPistHitbox;

	// Sniper
	CGroupBox WeaponSnipGroup;
	CSlider   WeaponSnipSpeed;
	CSlider   WeaponSnipFoV;
	CCheckBox WeaponSnipRecoil;
	CComboBox WeaponSnipHitbox;

	// Mod Key
	CGroupBox ModGroup;
	CKeyBind  ModKey;
	CSlider   ModSpeed;
	CSlider   ModFoV;
	CComboBox ModHitbox;

};

class CVisualsTab : public CTab
{
public:
	void Setup();

	CGroupBox grpOptions;
	CCheckBox OptionsBox;
	CCheckBox OptionsName;
	CCheckBox OptionsHealth;
	CCheckBox OptionsWeapon;
	CCheckBox OptionsInfo;
	CCheckBox OptionsSkeleton;
	CCheckBox OptionsAimSpot;
	CCheckBox OptionsCompRank;

	CGroupBox grpFilters;
	CCheckBox FiltersPlayers;
	CCheckBox FiltersEnemiesOnly;
	CCheckBox FiltersWeapons;
	CCheckBox FiltersChickens;
	CCheckBox FiltersC4;

	CGroupBox grpOther;
	CCheckBox OtherCrosshair;
	CCheckBox OtherRecoilCrosshair;
	CCheckBox OtherNoVisualRecoil;
	CCheckBox OtherHitmarker;
	CCheckBox OtherNoFlash;
	CCheckBox OtherNoSmoke;
	CCheckBox OtherNoHands;
	CComboBox cboChams;
};

class CMiscTab : public CTab
{
public:
	void Setup();

	CGroupBox grpMisc;
	CCheckBox OtherEnginePrediction;
	CCheckBox OtherBhop;
	CCheckBox OtherAutoStrafe;
	CComboBox cboChatSpam;
	CComboBox cboNameSpam;
	CKeyBind keyAirStuck;
	CCheckBox OtherServerLag;
	
	CGroupBox grpSkins;
	CComboBox SkinKnife;
};

class CPlayersTab : public CTab
{
public:
	void Setup();
	CGroupBox grpPList;
	CListBox lstPlayers;

	CCheckBox PlayerFriendly;
	CCheckBox PlayerAimPrio;
	CCheckBox PlayerCalloutSpam;

};

class CHvHTab : public CTab
{
public:
	void Setup();
	CGroupBox grpAA;
	CCheckBox AntiAimEnable;
	CComboBox AntiAimPitch;
	CComboBox AntiAimYaw;
	CCheckBox AccuracyAngleFix;

	CGroupBox grpHvH;
	CCheckBox FakeLag;
	CCheckBox AntiUntrusted;
	CCheckBox NoSpread;
};

class AyyWareWindow : public CWindow
{
public:
	void Setup();

	CRageTab RageBotTab;
	CLegitTab LegitBotTab;
	CVisualsTab VisualsTab;
	CMiscTab MiscTab;
	CPlayersTab PlayersTab;
	CHvHTab HvHTab;

	CComboBox SettingsFile;
	CLabel UserLabel;
};

namespace Menu
{
	extern AyyWareWindow Window;
	void DoFrame();
	void Initialize();
};