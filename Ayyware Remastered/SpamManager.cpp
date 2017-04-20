#include "SpamManager.h"
#include "Menu.h"
#include "HitMarker.h"

void ChatSpammer::DoSpamTick()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.5)
		return;

	std::string text = "FUCK";

	int SpamType = Menu::Window.MiscTab.cboChatSpam.GetIndex();
	if (SpamType)
	{
		if (SpamType == 1)
			text = GetRegularSpam();
		else if (SpamType == 2)
			text = GetPlayerSpam();
		else if (SpamType == 3 && HitMarker.EventMan.ShouldRoundStart())
			text = GetRegularSpam();
		else if (SpamType == 4)
			text = GetSinglePlayerSpam();
		if(text != "FUCK")
			SayInChat(text);
	}
	

	start_t = clock();
}

void ChatSpammer::DoNameSpamTick()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	std::string text = "FUCK";

	int SpamType = Menu::Window.MiscTab.cboNameSpam.GetIndex();
	static bool wasSpamming = false;
	static std::string nameBackup = "AUTISTICWARE.";

	if (SpamType > 0)
	{
		static bool useSpace = false;

		if (SpamType == 1) // AyyWare
			text = useSpace ? "卐卐 Hitler Done 9/11 卐卐 " : "卐卐 Hitler Done 9/11 卐卐";
		else if (SpamType == 2) //Name Steal
			text = GetNameChange() + " ";
		else if (SpamType == 3) //***ware sucks dick
			text = useSpace ? "***WARE AINT SHIT" : "***WARE AlNT SHIT";
		else if (SpamType == 4) //Name + Callout
			text = GetNameCalloutChange();
		if (text != "FUCK")
			SetName(text);

		useSpace = !useSpace;
	}
	else
	{

	}

	start_t = clock();
}

std::string ChatSpammer::GetRegularSpam()
{
	return "AUTISTICWARE - DANKEST AUTISM CHEATS IN THE WORLD";
}

std::string ChatSpammer::GetPlayerSpam()
{
	// Loop through all active entitys
	std::vector < std::string > Names;
	std::vector < IClientEntity* > Ents;
	bool EntsFound = false;


	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && HackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != HackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo) && entity->IsAlive())
				{
					if (!strstr(pInfo.name, "GOTV"))
					{
						Names.push_back(pInfo.name);
						Ents.push_back(entity);
						EntsFound = true;
					}
				}
			}
		}
	}

	if (EntsFound)
	{
		int randomIndex = rand() % Names.size();
		char buffer[256];
		sprintf_s(buffer, "%s Is At %s With %dHP", Names[randomIndex].c_str(), Ents[randomIndex]->GetLastPlaceName(), Ents[randomIndex]->GetHealth());
		return buffer;
	}
	else
		return "FUCK";
}

std::string ChatSpammer::GetSinglePlayerSpam()
{
	// Loop through all active entitys
	std::vector < std::string > Names;
	std::vector < IClientEntity* > Ents;
	bool EntsFound = false;


	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && HackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != HackManager.pLocal() && IsCalloutTarget(i))
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo) && entity->IsAlive())
				{
					if (!strstr(pInfo.name, "GOTV"))
					{
						Names.push_back(pInfo.name);
						Ents.push_back(entity);
						EntsFound = true;
					}
				}
			}
		}
	}

	if (EntsFound)
	{
		int randomIndex = rand() % Names.size();
		char buffer[256];
		sprintf_s(buffer, "%s Is At %s With %dHP", Names[randomIndex].c_str(), Ents[randomIndex]->GetLastPlaceName(), Ents[randomIndex]->GetHealth());
		return buffer;
	}
	else
		return "FUCK";
}

std::string ChatSpammer::GetNameChange()
{
	// Loop through all active entitys
	std::vector < std::string > Names;
	std::vector < IClientEntity* > Ents;
	bool EntsFound = false;
	static int lastEnt = -1;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && HackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != HackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo) && entity->IsAlive())
				{
					if (!strstr(pInfo.name, "GOTV"))
					{
						Names.push_back(pInfo.name);
						Ents.push_back(entity);
						EntsFound = true;
					}
				}
			}
		}
	}

	if (EntsFound)
	{
		int randomIndex = rand() % Names.size();
		if (Ents[randomIndex]->GetIndex() != lastEnt)
		{
			lastEnt = Ents[randomIndex]->GetIndex();
			return Names[randomIndex];
		}
		else
		{
			randomIndex = rand() % Names.size();
			if (Ents[randomIndex]->GetIndex() != lastEnt)
			{
				lastEnt = Ents[randomIndex]->GetIndex();
				return Names[randomIndex];
			}
		}
		
	}
	return "FUCK";
}

std::string ChatSpammer::GetNameCalloutChange()
{
	// Loop through all active entitys
	std::vector < std::string > Names;
	std::vector < IClientEntity* > Ents;
	bool EntsFound = false;
	static int lastEnt = -1;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && HackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != HackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo) && entity->IsAlive())
				{
					if (!strstr(pInfo.name, "GOTV"))
					{
						Names.push_back(pInfo.name);
						Ents.push_back(entity);
						EntsFound = true;
					}
				}
			}
		}
	}

	if (EntsFound)
	{
		int randomIndex = rand() % Names.size();
		if (Ents[randomIndex]->GetIndex() != lastEnt)
		{
			lastEnt = Ents[randomIndex]->GetIndex();
			char buffer[256];
			sprintf_s(buffer, "%s @ %s", Names[randomIndex].c_str(), Ents[randomIndex]->GetLastPlaceName());
			return buffer;
		}
		else
		{
			randomIndex = rand() % Names.size();
			if (Ents[randomIndex]->GetIndex() != lastEnt)
			{
				lastEnt = Ents[randomIndex]->GetIndex();
				char buffer[256];
				sprintf_s(buffer, "%s @ %s", Names[randomIndex].c_str(), Ents[randomIndex]->GetLastPlaceName());
				return buffer;
			}
		}

	}

	return "FUCK";
}