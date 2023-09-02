#include "pch.h"
#include "Hook.h"
#include "Hooks.h"
#include "Data.h"
#include "Player.h"
#include "PlayerList.h"
#include "Vars.h"
#include "Math.h"
#include "Draw.h"
#include "util.h"

extern CData g_data;
extern CDrawing g_drawing;

#define IS_RETADDR_INSIDE(x) (std::ranges::find(x, (DWORD)(_ReturnAddress()) - (DWORD)(g_data.GetSampDll())) != std::end(x))

static constexpr DWORD g_localPlayerSkinRetaddrs[] = { 0x3d37, 0x192e9 };
static constexpr DWORD g_localPlayerSkinBackupIgnoreRetaddrs[] = { 0x68D6A }; 
typedef int(__thiscall* SetPlayerSkinHook_t)(CPlayer*, int);
static SetPlayerSkinHook_t g_ogSetPlayerSkinHook = nullptr;
static int __fastcall SetPlayerSkinHook(CPlayer* self, DWORD, int skin)
{
	// these are probably a bit pedantic... just wanted to make sure there's no oddities
	if (IS_RETADDR_INSIDE(g_localPlayerSkinRetaddrs) && (self == g_data.GetLocalPlayer()))
	{
		// don't update backup if we are setting skin ourselves
		if (!IS_RETADDR_INSIDE(g_localPlayerSkinBackupIgnoreRetaddrs))
			g_data.SetBackupSkin(skin); 

		if (g_data.GetSkinChangerMode() == SkinChangerMode::On)
			skin = g_data.GetSkin();
	}

	Hook::Remove(SetPlayerSkinHook);
	int r = g_ogSetPlayerSkinHook(self, skin);
	Hook::Reinstall(SetPlayerSkinHook);
	
	return r;
}

typedef uint32_t SampARGB_t;
static constexpr DWORD g_nameTagRetaddrs[] = { 0x6b637 };
typedef int(__stdcall* DrawToScreenFn_t)(int, int, const char*, int, int, int, int);
static DrawToScreenFn_t g_ogDrawToScreen = nullptr;
static int __stdcall DrawToScreenHook(int a1, int a2, const char* str, int a4, int a5, int a6, SampARGB_t color)
{
	//if (str && *str)
	//	LOG(str << L' ' << std::hex << (DWORD)(_ReturnAddress()) << L'\n');

	//if (IS_RETADDR_INSIDE(g_nameTagRetaddrs))
	//	color = (0xff << 24) | (uint32_t)((uint64_t)(GetTickCount64() >> 40));

	Hook::Remove(DrawToScreenHook);
	int r = g_ogDrawToScreen(a1, a2, str, a4, a5, a6, color);
	Hook::Reinstall(DrawToScreenHook);

	return r;
}

typedef int(__cdecl* GtaSaEveryFrame_t)(int, int, int);
static GtaSaEveryFrame_t g_ogGtaSaEveryFrame = nullptr;
int __cdecl GtaSaEveryFrameHook(int a, int b, int c)
{
	// events
	g_data.UpdateSkinIfNeeded();

	// esp
	g_drawing.PushDrawables([](CDrawing* pDrawing) 
	{
		CQueue& queue = pDrawing->GetQueue();

		CPlayer* pLocalPlayer = g_data.GetLocalPlayer();
		if (!pLocalPlayer)
			return;

		// get local origin
		Vector3D vLocalOrigin;
		pLocalPlayer->GetOrigin(&vLocalOrigin);

		// turn local origin to screen position
		Vector3D vLocalScreen;
		g_data.WorldToScreen(&vLocalOrigin, &vLocalScreen);

		const POINT pointX
		{
			.x = (LONG)vLocalScreen[0],
			.y = (LONG)vLocalScreen[1]
		};

		std::vector<Line> lines;

		CPlayerList* pPlayerList = g_data.GetVars()->GetPlayerList();
		for (DWORD i = 0; i <= pPlayerList->GetPlayerCount(); i++)
		{
			CPlayer* pPlayer = pPlayerList->GetPlayer(i);

			if (pPlayer && pPlayer != pLocalPlayer)
			{
				for (int i = 0; i < 8; i++)
				{
					// get head origin
					Vector3D vHead;
					pPlayer->GetBonePosition(i, &vHead);

					// turn head origin to screen position
					Vector3D vScreen;
					g_data.WorldToScreen(&vHead, &vScreen);

					// draw

					const D3DRECT rect
					{
						.x1 = (LONG)(vScreen[0]) - 2,
						.y1 = (LONG)(vScreen[1]) - 2,
						.x2 = (LONG)(vScreen[0]) + 2,
						.y2 = (LONG)(vScreen[1]) + 2
					};
					queue.Push(new CRectangle(rect, D3DCOLOR_XRGB(255, 255, 0)));

					const POINT pointY
					{
						.x = (LONG)vScreen[0],
						.y = (LONG)vScreen[1]
					};

					lines.emplace_back(pointX, pointY);
				}
			}
		}

		//queue.Push(new CLines(std::move(lines), D3DCOLOR_XRGB(255, 0, 0)));
	});
	
	Hook::Remove(GtaSaEveryFrameHook);
	int r = g_ogGtaSaEveryFrame(a, b, c);
	Hook::Reinstall(GtaSaEveryFrameHook);

	return r;
}

typedef void(_stdcall* PresentFn_t)(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
static PresentFn_t g_ogPresent = nullptr;
static void __stdcall PresentHook(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	// render accumulated queue
	g_drawing.DrawQueue(pDevice);

	CText(POINT{ 0, 0 }, "test 123", FontStyle{ .font = "Arial", .size = 23, .weight = 0, .italic = true },
		D3DCOLOR_ARGB(255, 255, 255, 0)).Draw(pDevice);

	Hook::Remove(PresentHook);
	g_ogPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	Hook::Reinstall(PresentHook);
}

static constexpr DWORD g_fnGtaSaEveryFrame = 0x7F99B0; // can't disassemble in IDA, try ghidra decompiler
														// it's also stored inside "dword_C97B24 + 152" but
														// going there after, the fp is null
static constexpr DWORD g_fnSetPlayerSkin = 0xaff50;
static constexpr DWORD g_fnDrawToScreen = 0x73BE0;
void Hooks::Install()
{
	Hook::Install((void*)((DWORD)(g_data.GetGtaExe()) + g_fnGtaSaEveryFrame), GtaSaEveryFrameHook, (void**)&g_ogGtaSaEveryFrame);
	Hook::Install((void*)((DWORD)(g_data.GetSampDll()) + g_fnDrawToScreen), DrawToScreenHook, (void**)&g_ogDrawToScreen);
	Hook::Install((void*)((DWORD)(g_data.GetSampDll()) + g_fnSetPlayerSkin), SetPlayerSkinHook, (void**)&g_ogSetPlayerSkinHook);
	void* pPresent = (*(void***)(g_data.GetContext()->GetDevice()))[17];
	Hook::Install(pPresent, PresentHook, (void**)&g_ogPresent);
}
