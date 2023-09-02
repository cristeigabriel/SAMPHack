#include "pch.h"
#include "Init.h"
#include "Data.h"
#include "Hooks.h"

extern CData g_data;

void WINAPI Core::Init(void*)
{
#if _DEBUG
	AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif

	g_data.SetDebugMode(DebugMode::On);
	g_data.SetSkinChangerMode(SkinChangerMode::On);
	g_data.SetSkin(294);
	Hooks::Install();

	g_data.WriteToChat("{ffff00}b a i e t i   t r i s t i   w a r e");

	while (true)
	{
		Sleep(1000);
	}
}
