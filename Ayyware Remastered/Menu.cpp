/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#include "Menu.h"
#include "Controls.h"
#include "DLLMain.h"

AyyWareWindow Menu::Window;

void UpdatePlayerList();

std::map<DWORD, PlayerListItem_t> PlayerList;

void Menu::DoFrame()
{
	GUI.Update();
	GUI.Draw();

	UpdatePlayerList();
}

void Menu::Initialize()
{
	Window.Setup();

	GUI.RegisterWindow(&Window);
	GUI.BindWindow(VK_INSERT, &Window);
	
	Menu::Window.SetConfigFile(Menu::Window.SettingsFile.GetItem());
	GUI.LoadWindowState(&Window, Window.SettingsFile.GetItem());

	Utilities::Log("Ayyware 2.0 Menu Ready");
}

//-------------------------------------------------------------

void ReloadSettings(void)
{
	Menu::Window.SaveToCurrentConfig();
	Menu::Window.SetConfigFile(Menu::Window.SettingsFile.GetItem());
	GUI.LoadWindowState(&Menu::Window, Menu::Window.SettingsFile.GetItem());
}

void AyyWareWindow::Setup()
{
	SetPosition(50, 50);
	SetTitle("Ayyware 2.0");
	SetSize(900, 550);
	EnableTabs(true);

	RegisterTab(&RageBotTab);
	RegisterTab(&LegitBotTab);
	RegisterTab(&VisualsTab);
	RegisterTab(&MiscTab);
	RegisterTab(&PlayersTab);
	RegisterTab(&HvHTab);
	char userBuffer[120];
	
	RageBotTab.Setup();
	LegitBotTab.Setup();
	VisualsTab.Setup();
	MiscTab.Setup();
	PlayersTab.Setup();
	HvHTab.Setup();
}

void CRageTab::Setup()
{
	SetTitle("RageBot");

	grpAimbot.SetPosition(10, 10);
	grpAimbot.SetSize(680, 50);
	grpAimbot.SetText("Rage");
	grpAimbot.SetColumns(1);
	RegisterControl(&grpAimbot);

	RegisterControl(&Menu::Window.SettingsFile);
	RegisterControl(&Menu::Window.UserLabel);


	grpAimbot.PlaceLabledControl("Enable", this, &AimbotEnable, 0, 0);
	grpAimbot.PlaceLabledControl("Fire Weapon", this, &AimbotAutoFire, 0, 1);
	AimbotFov.SetBoundaries(0.1f, 180.f); AimbotFov.SetValue(38.f);
	grpAimbot.PlaceLabledControl("Field of View", this, &AimbotFov, 0, 2);
	grpAimbot.PlaceLabledControl("Silent Aim", this, &AimbotSilentAim, 0, 3);
	grpAimbot.PlaceLabledControl("Auto-Pistol", this, &AimbotAutoPistol, 0, 5);
	grpAimbot.PlaceLabledControl("Activate on Key", this, &AimbotKeyPress, 0, 6);
	grpAimbot.PlaceLabledControl("Key Bind", this, &AimbotKeyBind, 0, 7);
	AimbotAimstep.AddItem("Off");
	AimbotAimstep.AddItem("Normal");
	AimbotAimstep.AddItem("Slower");
	grpAimbot.PlaceLabledControl("Aim-Step", this, &AimbotAimstep, 0, 8);

	grpTarget.SetPosition(10, GetNextGroupboxY(&grpAimbot));
	grpTarget.SetSize(680, 50);
	grpTarget.SetText("Targetting");
	grpTarget.SetColumns(2);
	RegisterControl(&grpTarget);

	TargetSelection.AddItem("CrossHair");
	TargetSelection.AddItem("Distance");
	TargetSelection.AddItem("Lowest HP");
	grpTarget.PlaceLabledControl("Selection Style", this, &TargetSelection, 0, 0);
	TargetFriendlyFire.AddItem("Only Enemies");
	TargetFriendlyFire.AddItem("All Players");
	grpTarget.PlaceLabledControl("Teams", this, &TargetFriendlyFire, 1, 0);
	TargetHitbox.AddItem("Head");
	TargetHitbox.AddItem("Neck");
	TargetHitbox.AddItem("Chest");
	TargetHitbox.AddItem("Stomach");
	grpTarget.PlaceLabledControl("HitBox", this, &TargetHitbox, 0, 1);
	TargetHitscan.AddItem("Off"); //0
	TargetHitscan.AddItem("Head / Body"); // 1
	TargetHitscan.AddItem("Basic"); // 2
	TargetHitscan.AddItem("Every fucking thing"); // 3
	grpTarget.PlaceLabledControl("HitScan", this, &TargetHitscan, 1, 1);

	grpAccuracy.SetPosition(10, GetNextGroupboxY(&grpTarget));
	grpAccuracy.SetSize(680, 50);
	grpAccuracy.SetText("Accuracy");
	grpAccuracy.SetColumns(1);
	RegisterControl(&grpAccuracy);

	grpAccuracy.PlaceLabledControl("No Recoil", this, &AccuracyRecoil, 0, 0);
	grpAccuracy.PlaceLabledControl("AyyWall", this, &AccuracyAutoWall, 0, 1);
	grpAccuracy.PlaceLabledControl("Stop Move", this, &AccuracyAutoStop, 0, 2);
	grpAccuracy.PlaceLabledControl("Crouch", this, &AccuracyAutoCrouch, 0, 3);
}

void CLegitTab::Setup()
{
	SetTitle("Legit");
	RegisterControl(&Menu::Window.SettingsFile);
	RegisterControl(&Menu::Window.UserLabel);


	AimbotGroup.SetPosition(10, 10);
	AimbotGroup.SetSize(680, 50);
	AimbotGroup.SetText("Aim boat");
	AimbotGroup.SetColumns(2);
	RegisterControl(&AimbotGroup);

	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable, 0, 0);
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire, 1, 0);
	AimbotGroup.PlaceLabledControl("Use Key", this, &AimbotKeyPress, 0, 1);
	AimbotGroup.PlaceLabledControl("Key-Bind", this, &AimbotKeyBind, 1, 1);
	AimbotGroup.PlaceLabledControl("AutoPistol", this, &AimbotAutoPistol, 0, 2);
	AimbotInaccuracy.SetBoundaries(0.f, 100.f);
	AimbotGroup.PlaceLabledControl("Inaccuracy", this, &AimbotInaccuracy, 1, 2);


	WeaponMainGroup.SetPosition(10, GetNextGroupboxY(&AimbotGroup));
	WeaponMainGroup.SetSize(680, 50);
	WeaponMainGroup.SetText("Rifles and Misc");
	WeaponMainGroup.SetColumns(2);
	RegisterControl(&WeaponMainGroup);

	WeaponMainSpeed.SetBoundaries(0.001f, 1.f);
	WeaponMainSpeed.SetValue(0.1f);
	WeaponMainGroup.PlaceLabledControl("Speed", this, &WeaponMainSpeed, 0, 0);
	WeaponMainFoV.SetBoundaries(0.1f, 20.f);
	WeaponMainFoV.SetValue(2.f);
	WeaponMainGroup.PlaceLabledControl("FoV", this, &WeaponMainFoV, 1, 0);
	WeaponMainRecoil.SetState(true);
	WeaponMainGroup.PlaceLabledControl("Recoil", this, &WeaponMainRecoil, 0, 1);
	WeaponMainHitbox.AddItem("Head");
	WeaponMainHitbox.AddItem("Neck");
	WeaponMainHitbox.AddItem("Chest");
	WeaponMainHitbox.AddItem("Stomach");
	WeaponMainGroup.PlaceLabledControl("HitBox", this, &WeaponMainHitbox, 1, 1);

	WeaponPistGroup.SetPosition(10, GetNextGroupboxY(&WeaponMainGroup));
	WeaponPistGroup.SetSize(680, 50);
	WeaponPistGroup.SetText("Pistols");
	WeaponPistGroup.SetColumns(2);
	RegisterControl(&WeaponPistGroup);

	WeaponPistSpeed.SetBoundaries(0.001f, 1.f);
	WeaponPistSpeed.SetValue(0.1f);
	WeaponPistGroup.PlaceLabledControl("Speed", this, &WeaponPistSpeed, 0, 0);
	WeaponPistFoV.SetBoundaries(0.1f, 20.f);
	WeaponPistFoV.SetValue(2.f);
	WeaponPistGroup.PlaceLabledControl("FoV", this, &WeaponPistFoV, 1, 0);
	WeaponPistGroup.PlaceLabledControl("Recoil", this, &WeaponPistRecoil, 0, 1);
	WeaponPistHitbox.AddItem("Head");
	WeaponPistHitbox.AddItem("Neck");
	WeaponPistHitbox.AddItem("Chest");
	WeaponPistHitbox.AddItem("Stomach");
	WeaponPistGroup.PlaceLabledControl("HitBox", this, &WeaponPistHitbox, 1, 1);

	WeaponSnipGroup.SetPosition(10, GetNextGroupboxY(&WeaponPistGroup));
	WeaponSnipGroup.SetSize(680, 50);
	WeaponSnipGroup.SetText("Snipers");
	WeaponSnipGroup.SetColumns(2);
	RegisterControl(&WeaponSnipGroup);

	WeaponSnipSpeed.SetBoundaries(0.001f, 1.f);
	WeaponSnipSpeed.SetValue(0.1f);
	WeaponSnipGroup.PlaceLabledControl("Speed", this, &WeaponSnipSpeed, 0, 0);
	WeaponSnipFoV.SetBoundaries(0.1f, 20.f);
	WeaponSnipFoV.SetValue(2.f);
	WeaponSnipGroup.PlaceLabledControl("FoV", this, &WeaponSnipFoV, 1, 0);
	WeaponSnipGroup.PlaceLabledControl("Recoil", this, &WeaponSnipRecoil, 0, 1);
	WeaponSnipHitbox.AddItem("Head");
	WeaponSnipHitbox.AddItem("Neck");
	WeaponSnipHitbox.AddItem("Chest");
	WeaponSnipHitbox.AddItem("Stomach");
	WeaponSnipGroup.PlaceLabledControl("HitBox", this, &WeaponSnipHitbox, 1, 1);

	ModGroup.SetPosition(10, GetNextGroupboxY(&WeaponSnipGroup));
	ModGroup.SetSize(680, 50);
	ModGroup.SetText("Modifier Key");
	ModGroup.SetColumns(2);
	RegisterControl(&ModGroup);

	ModGroup.PlaceLabledControl("Key-Bind", this, &ModKey, 0, 0);
	ModSpeed.SetBoundaries(0.001f, 1.f);
	ModSpeed.SetValue(0.1f);
	ModGroup.PlaceLabledControl("Speed", this, &ModSpeed, 0, 1);
	ModFoV.SetBoundaries(0.1f, 20.f);
	ModFoV.SetValue(2.f);
	ModGroup.PlaceLabledControl("FoV", this, &ModFoV, 1, 1);
	ModHitbox.AddItem("Head");
	ModHitbox.AddItem("Neck");
	ModHitbox.AddItem("Chest");
	ModHitbox.AddItem("Stomach");
	ModGroup.PlaceLabledControl("HitBox", this, &ModHitbox, 0, 2);

}

void CVisualsTab::Setup()
{
	SetTitle("Visuals");
	RegisterControl(&Menu::Window.SettingsFile);
	RegisterControl(&Menu::Window.UserLabel);


	grpOptions.SetPosition(10, 10);
	grpOptions.SetSize(680, 50);
	grpOptions.SetText("Options");
	grpOptions.SetColumns(2);
	RegisterControl(&grpOptions);

	grpOptions.PlaceLabledControl("Draw Box", this,	&OptionsBox,			0, 0);
	grpOptions.PlaceLabledControl("Draw Name", this, &OptionsName,			1, 0);
	grpOptions.PlaceLabledControl("Draw Health", this, &OptionsHealth,		0, 1);
	grpOptions.PlaceLabledControl("Draw Weapon", this, &OptionsWeapon,		1, 1);
	grpOptions.PlaceLabledControl("Draw Extra Info", this, &OptionsInfo,	0, 2);
	grpOptions.PlaceLabledControl("Draw Bones", this, &OptionsSkeleton,		1, 2);
	grpOptions.PlaceLabledControl("Draw Aim Spot", this, &OptionsAimSpot,	0, 3);
	grpOptions.PlaceLabledControl("Draw Rank", this, &OptionsCompRank,		1, 3);

	grpFilters.SetPosition(10, GetNextGroupboxY(&grpOptions));
	grpFilters.SetSize(680, 50);
	grpFilters.SetText("Object Filters");
	grpFilters.SetColumns(1);
	RegisterControl(&grpFilters);

	grpFilters.PlaceLabledControl("Show Players", this, &FiltersPlayers, 0, 0);
	grpFilters.PlaceLabledControl("Only Show Enemies", this, &FiltersEnemiesOnly, 0, 1);
	grpFilters.PlaceLabledControl("Show Dropped Items", this, &FiltersWeapons, 0, 2);
	grpFilters.PlaceLabledControl("Show Chickens", this, &FiltersChickens, 0, 3);
	grpFilters.PlaceLabledControl("Show Bomb", this, &FiltersC4, 0, 4);

	grpOther.SetPosition(10, GetNextGroupboxY(&grpFilters));
	grpOther.SetSize(680, 50);
	grpOther.SetText("Other Visuals");
	grpOther.SetColumns(1);
	RegisterControl(&grpOther);

	cboChams.AddItem("None");
	cboChams.AddItem("Normal");
	cboChams.AddItem("Flat");
	grpOther.PlaceLabledControl("Chams", this, &cboChams, 0, 0);
	grpOther.PlaceLabledControl("No Visual Recoil", this, &OtherNoVisualRecoil, 0, 1);
	grpOther.PlaceLabledControl("Recoil Crosshair", this, &OtherRecoilCrosshair, 0, 2);
	grpOther.PlaceLabledControl("CrossHair", this, &OtherCrosshair, 0, 3);
	grpOther.PlaceLabledControl("No Flash", this, &OtherNoFlash, 0, 4);
	grpOther.PlaceLabledControl("No Smoke", this, &OtherNoSmoke, 0, 5);
	grpOther.PlaceLabledControl("No Hands", this, &OtherNoHands, 0, 6);
	grpOther.PlaceLabledControl("HitMarkers", this, &OtherHitmarker, 0, 7);
	
}

void CMiscTab::Setup()
{
	SetTitle("Misc");
	RegisterControl(&Menu::Window.SettingsFile);
	RegisterControl(&Menu::Window.UserLabel);


	grpMisc.SetPosition(10, 10);
	grpMisc.SetSize(680, 50);
	grpMisc.SetText("Misc");
	grpMisc.SetColumns(1);
	RegisterControl(&grpMisc);

	grpMisc.PlaceLabledControl("Engine Pred", this, &OtherEnginePrediction, 0, 0);
	grpMisc.PlaceLabledControl("Bunny Hop", this, &OtherBhop, 0, 1);
	grpMisc.PlaceLabledControl("Auto Strafe", this, &OtherAutoStrafe, 0, 2);
	cboChatSpam.AddItem("Off");
	cboChatSpam.AddItem("Regular");
	cboChatSpam.AddItem("Team Callout");
	cboChatSpam.AddItem("Round-Say");
	cboChatSpam.AddItem("Playerlist Callout");
	grpMisc.PlaceLabledControl("Chet Spam", this, &cboChatSpam, 0, 3);
	cboNameSpam.AddItem("None");
	cboNameSpam.AddItem("AUTIST");
	cboNameSpam.AddItem("NameSteal");
	cboNameSpam.AddItem("Steal+Callout");
	grpMisc.PlaceLabledControl("Name Spam", this, &cboNameSpam, 0, 4);
	grpMisc.PlaceLabledControl("Air Stuck", this, &keyAirStuck, 0, 5);
	grpMisc.PlaceLabledControl("Lag Exploit", this, &OtherServerLag, 0, 6);


	grpSkins.SetPosition(10, GetNextGroupboxY(&grpMisc));
	grpSkins.SetSize(680, 50);
	grpSkins.SetText("Skins");
	grpSkins.SetColumns(1);
	RegisterControl(&grpSkins);

	SkinKnife.AddItem("None");
	SkinKnife.AddItem("Bayonet");
	SkinKnife.AddItem("Butterfly");
	SkinKnife.AddItem("Flip");
	SkinKnife.AddItem("Gun Game");
	SkinKnife.AddItem("Gut");
	SkinKnife.AddItem("Karambit");
	SkinKnife.AddItem("M9");
	SkinKnife.AddItem("Huntsman");
	SkinKnife.AddItem("Falchion");
	SkinKnife.AddItem("Dagger");
	grpSkins.PlaceLabledControl("Knife", this, &SkinKnife, 0, 0);


}

void CHvHTab::Setup()
{
	SetTitle("HvH");
	RegisterControl(&Menu::Window.SettingsFile);
	RegisterControl(&Menu::Window.UserLabel);


	grpAA.SetPosition(10, 10);
	grpAA.SetSize(680, 50);
	grpAA.SetText("Anti-Ayym");
	grpAA.SetColumns(2);
	RegisterControl(&grpAA);

	grpAA.PlaceLabledControl("Enable", this, &AntiAimEnable, 0, 0);
	AntiAimPitch.AddItem("None"); //0
	AntiAimPitch.AddItem("Up"); //1
	AntiAimPitch.AddItem("Down");  //2
	AntiAimPitch.AddItem("Emotion"); //3
	AntiAimPitch.AddItem("Jitter"); //4
	AntiAimPitch.AddItem("Jitter 2"); //5
	AntiAimPitch.AddItem("FayykeDown"); //6
	grpAA.PlaceLabledControl("Pitch", this, &AntiAimPitch, 0, 1);

	AntiAimYaw.AddItem("None"); //0
	AntiAimYaw.AddItem("Spin Fast"); //1
	AntiAimYaw.AddItem("Spin Slow"); //2
	AntiAimYaw.AddItem("Inverse"); //3
	AntiAimYaw.AddItem("Jitter"); //4
	AntiAimYaw.AddItem("Jitter 2"); //5
	AntiAimYaw.AddItem("Flip"); //6
	AntiAimYaw.AddItem("Edge"); //7

	grpAA.PlaceLabledControl("Yaw", this, &AntiAimYaw, 1, 1);

	grpHvH.SetPosition(10, GetNextGroupboxY(&grpAA));
	grpHvH.SetSize(680, 50);
	grpHvH.SetText("Hack vs Hack");
	grpHvH.SetColumns(2);
	RegisterControl(&grpHvH);
	
	AntiUntrusted.SetState(true);
	grpHvH.PlaceLabledControl("Anti-Untrusted", this, &AntiUntrusted, 0, 7);
	grpHvH.PlaceLabledControl("No Spread", this, &NoSpread, 0, 8);
	grpHvH.PlaceLabledControl("Fakelag", this, &FakeLag, 0, 9);
	grpHvH.PlaceLabledControl("Resolver", this, &AccuracyAngleFix, 0, 10);

}

void CPlayersTab::Setup()
{
	SetTitle("PlayerList");
	RegisterControl(&Menu::Window.SettingsFile);
	RegisterControl(&Menu::Window.UserLabel);


	grpPList.SetPosition(10, 10);
	grpPList.SetSize(680, 200);
	grpPList.SetText("PlayerList");
	grpPList.SetColumns(2);
	RegisterControl(&grpPList);

	lstPlayers.SetPosition(20, 40);
	lstPlayers.SetSize(640, 50);
	lstPlayers.SetHeightInItems(20);
	RegisterControl(&lstPlayers);

	grpPList.SetSize(680, 500);

	grpPList.PlaceLabledControl("Friendly", this, &PlayerFriendly, 0, 17);
	grpPList.PlaceLabledControl("Aim-Priorety", this, &PlayerAimPrio, 1, 17);
	grpPList.PlaceLabledControl("Callout Spam", this, &PlayerCalloutSpam, 0, 18);

}

DWORD GetPlayerListIndex(int EntId)
{
	player_info_t pinfo;
	Interfaces::Engine->GetPlayerInfo(EntId, &pinfo);

	// Bot
	if (pinfo.guid[0] == 'B' && pinfo.guid[1] == 'O')
	{
		char buf[64]; sprintf_s(buf, "BOT_420%sAY", pinfo.name);
		return CRC32(buf, 64);
	}
	else
	{
		return CRC32(pinfo.guid, 32);
	}
}

bool IsFriendly(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].Friendly;
	}

	return false;
}

bool IsAimPrio(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].AimPrio;
	}

	return false;
}

bool IsCalloutTarget(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].Callout;
	}

	return false;
}

void UpdatePlayerList()
{
	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
	{
		Menu::Window.PlayersTab.lstPlayers.ClearItems();

		// Loop through all active entitys
		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
		{
			// Get the entity
			
			player_info_t pinfo;
			if (i != Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerInfo(i, &pinfo))
			{
				IClientEntity* pEntity = Interfaces::EntList->GetClientEntity(i);
				int HP = 100; char* Location = "Unknown";
				char *Friendly = " ", *AimPrio = " ";
				
				DWORD plistId = GetPlayerListIndex(Menu::Window.PlayersTab.lstPlayers.GetValue());
				if (PlayerList.find(plistId) != PlayerList.end())
				{
					Friendly = PlayerList[plistId].Friendly?"Friendly":"";
					AimPrio = PlayerList[plistId].AimPrio?"AimPrio":"";
				}
				 
			    if (pEntity && !pEntity->IsDormant())
				{
					HP = pEntity->GetHealth();
					Location = pEntity->GetLastPlaceName();
				}
				
				char nameBuffer[512];
				sprintf_s(nameBuffer, "%-24s %-10s %-10s [%d HP] [Last Seen At %s]", pinfo.name, IsFriendly(i)?"Friend":" ", IsAimPrio(i)?"AimPrio":" ", HP, Location);
				Menu::Window.PlayersTab.lstPlayers.AddItem(nameBuffer, i);
				
			}
			
		}

		DWORD meme = GetPlayerListIndex(Menu::Window.PlayersTab.lstPlayers.GetValue());

		// Have we switched to a different player?
		static int PrevSelectedPlayer = 0;
		if (PrevSelectedPlayer != Menu::Window.PlayersTab.lstPlayers.GetValue())
		{
			if (PlayerList.find(meme) != PlayerList.end())
			{
				Menu::Window.PlayersTab.PlayerFriendly.SetState(PlayerList[meme].Friendly);
				Menu::Window.PlayersTab.PlayerAimPrio.SetState(PlayerList[meme].AimPrio);
				Menu::Window.PlayersTab.PlayerCalloutSpam.SetState(PlayerList[meme].Callout);

			}
			else
			{
				Menu::Window.PlayersTab.PlayerFriendly.SetState(false);
				Menu::Window.PlayersTab.PlayerAimPrio.SetState(false);
				Menu::Window.PlayersTab.PlayerCalloutSpam.SetState(false);

			}
		}
		PrevSelectedPlayer = Menu::Window.PlayersTab.lstPlayers.GetValue();

		PlayerList[meme].Friendly = Menu::Window.PlayersTab.PlayerFriendly.GetState();
		PlayerList[meme].AimPrio = Menu::Window.PlayersTab.PlayerAimPrio.GetState();
		PlayerList[meme].Callout = Menu::Window.PlayersTab.PlayerCalloutSpam.GetState();

	}
}