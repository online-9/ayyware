/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Interfaces.h"
#include "Utilities.h"

bool IsBallisticWeapon(void* weapon);
bool IsPistol(void* weapon);
bool IsSniper(void* weapon);
int GetPlayerCompRank(IClientEntity* pEntity);
void SayInChat(std::string text);
void SetName(std::string name);