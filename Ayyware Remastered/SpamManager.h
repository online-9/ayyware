#pragma once
#include "Interfaces.h"

namespace ChatSpammer
{
	void DoSpamTick();
	void DoNameSpamTick();

	std::string GetRegularSpam();
	std::string GetCustomSpam();
	std::string GetPlayerSpam();
	std::string GetSinglePlayerSpam();

	std::string GetNameChange();
	std::string GetNameCalloutChange();


};