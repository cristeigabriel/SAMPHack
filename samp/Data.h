#pragma once

#include "util.h"
#include "Context.h"

class CPlayerController;
class CPlayer;
class CVars;
struct Vector3D;
class CDx;
typedef void(__cdecl* FormatFn_t)(DWORD, const char*, ...);
typedef CPlayer* (__thiscall* GetPlayerFn_t)(CPlayerController*);

enum class DebugMode : bool
{
	On = true,
	Off = false
};

enum class SkinChangerMode : bool
{
	On = true,
	Off = false
};

enum class UpdateSkinMode : uint8_t
{
	None = 0,
	Backup = 1,
	New = 2
};

class CData
{
public:
	CData();

	HMODULE GetSampDll() const;
	HMODULE GetGtaExe() const;

	CContext* GetContext() const;

	template <typename... Args>
	void WriteToChat(const char* fmt, Args&&... args) const;

	// allows you extra commands and unlocks some functions
	void SetDebugMode(DebugMode);
	DebugMode GetDebugMode() const;

	//  skin changer functionality
	void SetSkinChangerMode(SkinChangerMode);
	SkinChangerMode GetSkinChangerMode() const;
	void SetBackupSkin(int);
	int GetBackupSkin() const;
	void SetSkin(int);
	int GetSkin() const;
	void UpdateSkinIfNeeded();

	CPlayer* GetPlayer(CPlayerController*) const;
	CPlayer* GetLocalPlayer() const;

	CVars* GetVars() const;

	void WorldToScreen(const Vector3D*, Vector3D*);

private:
	HMODULE m_hmSampDll;
	HMODULE m_hmGtaExe;
	CContext* m_pContext;
	FormatFn_t m_fnWriteToChat;
	int* m_iDebugMode;
	SkinChangerMode m_skinChangerMode;
	int m_iSkin;
	int m_iBackupSkin;
	UpdateSkinMode m_updateSkinMode;
	GetPlayerFn_t m_fnGetPlayer;
	CPlayerController* m_pLocalPlayerController;
	CVars* m_pVars;
	CDx* m_pDx;
};

#include "Data.inl"
