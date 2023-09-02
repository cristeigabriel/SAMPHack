#include "pch.h"
#include "Rendering.h"

void CRendering::GetViewport(D3DVIEWPORT9* v)
{
	typedef void(__stdcall* GetViewportFn_t)(CRendering*, D3DVIEWPORT9*);
	GetViewportFn_t fn = (GetViewportFn_t)(((*(DWORD**)(this))[48]));
	fn(this, v);
}
