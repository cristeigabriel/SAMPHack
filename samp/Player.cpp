#include "pch.h"
#include "Player.h"
#include "Data.h"
#include "Math.h"

extern CData g_data;

static constexpr DWORD g_fnGetBonePosition = 0xAE480;
static constexpr DWORD g_fnIsInCar = 0xABE70;
static constexpr DWORD g_fnDistanceToLocal = 0x9F190;

bool CPlayer::IsInCar() const
{
	typedef int(__thiscall* IsInCarFn_t)(const CPlayer*);
	static IsInCarFn_t fnIsInCar = (IsInCarFn_t)((DWORD)(g_data.GetSampDll()) + g_fnIsInCar);
	return (bool)(fnIsInCar(this) & 1);
}

void CPlayer::GetOrigin(Vector3D* vTo) const
{
	const DWORD* pPlayer = (const DWORD*)this;
	int v7 = pPlayer[169]; // 0x68AFF
	const float* v8 = *(const float**)(v7 + 20);
	(*vTo)[0] = v8[12];
	(*vTo)[1] = v8[13];
	(*vTo)[2] = v8[14];
}

void CPlayer::GetBonePosition(int bone, Vector3D* pDest) const
{
	typedef void(__thiscall* GetBonePositionFn_t)(const CPlayer*, int, float*);
	static GetBonePositionFn_t fnGetBonePosition = (GetBonePositionFn_t)((DWORD)(g_data.GetSampDll()) + g_fnGetBonePosition);
	fnGetBonePosition(this, bone, &(*pDest)[0]);
}

long double CPlayer::GetDistanceToLocal() const
{
	typedef long double(__thiscall* GetDistanceToLocalFn_t)(const CPlayer*);
	static GetDistanceToLocalFn_t fnGetDistanceToLocal = (GetDistanceToLocalFn_t)((DWORD)(g_data.GetSampDll()) + g_fnDistanceToLocal);
	return fnGetDistanceToLocal(this);
}
