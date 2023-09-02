#include "pch.h"
#include "Vars.h"
#include "PlayerList.h"

// Offsets
static constexpr DWORD g_getNameTagStatus = 0x234;

void CVars::SetNameTagStatus(BYTE s)
{
	GetNameTagStatusEnabled() = s;
}

const BYTE& CVars::GetNameTagStatusEnabled() const
{
	return ((CVars*)(this))->GetNameTagStatusEnabled();
}

BYTE& CVars::GetNameTagStatusEnabled()
{
	return *(BYTE*)((DWORD)(this) + g_getNameTagStatus);
}

CPlayerList* CVars::GetPlayerList() const
{
	return *(CPlayerList**)(*(DWORD*)((DWORD)(this) + 990) + 4);
}