#include "pch.h"
#include "PlayerList.h"
#include "Data.h"

extern CData g_data;

// Offsets
static constexpr DWORD g_fnGetNameFromId = 0x175c0;

DWORD CPlayerList::GetPlayerCount() const
{
	return *(DWORD *)((DWORD)(this) + 12090);
}

CPlayer* CPlayerList::GetPlayer(DWORD dwIndex) const
{
	int v3 = *(DWORD *)((DWORD)(this) + 4 * (unsigned __int16)dwIndex + 8074); // 0x753AB
	if (v3)
	{
		int v4 = *(DWORD *)(v3 + 16);
		if (v4)
		{
			return (CPlayer*)*(DWORD*)(v4 + 477);
		}
	}

	return nullptr;
}

const char* CPlayerList::GetNameFromID(int id) const
{
	typedef const char* (__thiscall*GetNameFromIdFn_t)(const CPlayerList*, int);
	static GetNameFromIdFn_t fnGetNameFromId = (GetNameFromIdFn_t)((DWORD)(g_data.GetSampDll()) + g_fnGetNameFromId);
	return fnGetNameFromId(this, id);
}
