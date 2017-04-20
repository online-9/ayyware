/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Common.h"
#include "LoaderInfo.h"

// Macro to make stuff a little easier
#define DLL_EXPORT extern "C" __declspec(dllexport)

// -----------------------------------------------------------
// Namespace for interacting with the cheat backend and getting
// hold of some information
//
namespace BackEndInfo
{
	extern bool g_LoaderInfoSet;
	extern LoaderInfo g_LoaderInfo;
	extern HINSTANCE g_hModule;
	extern bool g_bUnloadModule;
};
