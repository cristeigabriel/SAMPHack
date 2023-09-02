#pragma once

#include "util.h"

class CContext;

typedef void(__thiscall* PushToKillfeedFn_t)(CContext*, const char*, const char*, int, int, int); // context, name killer, name killed, team killer, team killed, gun

class CContext
{
public:
	CContext() = delete;

	void PushToKillfeed(const char*, const char*, int, int, int);

	IDirect3DDevice9* GetDevice() const;
};
