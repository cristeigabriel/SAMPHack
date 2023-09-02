#include "pch.h"
#include "Data.h"
#include "Context.h"
#include "Player.h"
#include "PlayerController.h"

// Offsets
static constexpr DWORD g_writeToChat = 0x68070;
static constexpr DWORD g_context = 0x26EB88;
static constexpr DWORD g_debugMode = 0x26DFE8;
static constexpr DWORD g_fnGetPlayer = 0x1010;
static constexpr DWORD g_localPlayer = 0x26EBAC;
static constexpr DWORD g_vars = 0x26EB94;
static constexpr DWORD g_rendering = 0x26EB5C;
static constexpr DWORD g_worldToScreen = 0x71C20;
static constexpr DWORD g_dx = 0x26EB60;

CData::CData() 
{
	m_hmSampDll = GetModuleHandle(L"samp");
	m_hmGtaExe = GetModuleHandle(L"gta_sa");
	m_fnWriteToChat = (FormatFn_t)((DWORD)(m_hmSampDll)+g_writeToChat);
	m_pContext = *(CContext**)((DWORD)(m_hmSampDll)+g_context);
	m_iDebugMode = (int*)((DWORD)(m_hmSampDll)+g_debugMode);
	m_skinChangerMode = SkinChangerMode::Off;
	m_iSkin = -1;
	m_iBackupSkin = -1;
	m_updateSkinMode = UpdateSkinMode::None;
	m_fnGetPlayer = (GetPlayerFn_t)((DWORD)(m_hmSampDll)+g_fnGetPlayer);
	m_pLocalPlayerController = *(CPlayerController**)((DWORD)(m_hmSampDll)+g_localPlayer);
	m_pVars = *(CVars**)((DWORD)(m_hmSampDll)+g_vars);
	m_pDx = *(CDx**)((DWORD)(m_hmSampDll)+g_dx);
}

HMODULE CData::GetSampDll() const
{
	return m_hmSampDll;
}

HMODULE CData::GetGtaExe() const
{
	return m_hmGtaExe;
}

CContext* CData::GetContext() const
{
	return m_pContext;
}

void CData::SetDebugMode(DebugMode s)
{
	*m_iDebugMode = (s == DebugMode::On) ? 1 : 0;
}

DebugMode CData::GetDebugMode() const
{
	if ((*m_iDebugMode) > 0)
		return DebugMode::On;

	return DebugMode::Off;
}

void CData::SetSkinChangerMode(SkinChangerMode s)
{
	SkinChangerMode modeOld = m_skinChangerMode;
	m_skinChangerMode = s;
	if (m_skinChangerMode != modeOld)
	{
		if (modeOld == SkinChangerMode::On)
			m_updateSkinMode = UpdateSkinMode::Backup;
		else
			m_updateSkinMode = UpdateSkinMode::New;
	}
}

SkinChangerMode CData::GetSkinChangerMode() const
{
	return m_skinChangerMode;
}

void CData::SetBackupSkin(int skin)
{
	m_iBackupSkin = skin;
}

int CData::GetBackupSkin() const
{
	return m_iBackupSkin;
}

void CData::SetSkin(int skin)
{
	m_iSkin = skin;
	if (GetSkinChangerMode() == SkinChangerMode::On)
		m_updateSkinMode = UpdateSkinMode::New;
}

int CData::GetSkin() const
{
	return m_iSkin;
}

void CData::UpdateSkinIfNeeded()
{
	static constexpr DWORD g_setPlayerSkin = 0x68D00;
	if (m_updateSkinMode != UpdateSkinMode::None)
	{
		int skin = (m_updateSkinMode == UpdateSkinMode::Backup) ? m_iBackupSkin : m_iSkin;
		// we use this for a return address
		// we could also just encode our intent in skin instead to make sure we are looking at our own call...
		// but that isn't really worth it.
		typedef void(__cdecl* SetPlayerSkinFn_t)(const char*);
		static SetPlayerSkinFn_t fnSetPlayerSkin = (SetPlayerSkinFn_t)((DWORD)(m_hmSampDll)+g_setPlayerSkin);
		fnSetPlayerSkin(std::to_string(skin).c_str());

#if _DEBUG
		WriteToChat("{ffff00}SkinChanger: {ff00ff}Updated skin to {ffffff}%d", skin);
#endif
		
		m_updateSkinMode = UpdateSkinMode::None;
	}
}

CPlayer* CData::GetPlayer(CPlayerController* pPlayerController) const
{
	return m_fnGetPlayer(pPlayerController);
}

CPlayer* CData::GetLocalPlayer() const
{
	return GetPlayer(m_pLocalPlayerController);
}

CVars* CData::GetVars() const
{
	return m_pVars;
}

void CData::WorldToScreen(const Vector3D* pSrc, Vector3D* pOut)
{
	typedef void(__thiscall* WorldToScreenFn_t)(CDx*, const Vector3D*, Vector3D*);
	static WorldToScreenFn_t fnWorldtoScreen = (WorldToScreenFn_t)((DWORD)(m_hmSampDll)+g_worldToScreen);
	fnWorldtoScreen(m_pDx, pSrc, pOut);
}

CData g_data{};
