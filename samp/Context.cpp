#include "pch.h"
#include "Context.h"
#include "Data.h"

extern CData g_data;

static constexpr DWORD g_fnPushToKillfeed = 0x06A6B0;

void CContext::PushToKillfeed(const char* nameKiller, const char* nameKilled, int teamKiller, int teamKilled, int gun)
{
    static PushToKillfeedFn_t fnPushToKillfeed = (PushToKillfeedFn_t)((DWORD)(g_data.GetSampDll()) + g_fnPushToKillfeed);
    fnPushToKillfeed(this, nameKiller, nameKilled, teamKiller, teamKilled, gun);
}

IDirect3DDevice9* CContext::GetDevice() const
{
    return *(IDirect3DDevice9**)((DWORD)(this) + 327);
}
