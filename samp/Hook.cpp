#include "pch.h"
#include "Hook.h"

static std::unordered_map<void*, void*> g_mapDetourBack;
static_assert(sizeof(uintptr_t) == 4, "Not supported");
static constexpr size_t g_nBytesPatchForHook = sizeof(BYTE) + sizeof(uintptr_t);
typedef std::array<BYTE, g_nBytesPatchForHook> HookBytes_t;
static std::unordered_map<void*, HookBytes_t> g_mapOriginalBytes;

template <size_t N>
inline std::array<BYTE, N> ReadNBytes(void* at)
{
	std::array<BYTE, N>	bytes;
	for (auto i = 0; i < N; i++)
		bytes[i] = *(BYTE*)((DWORD)(at)+i);

	return bytes;
}

template <size_t N>
inline void WriteNBytes(void* at, const std::array<BYTE, N>& bytes)
{
	for (auto i = 0; i < N; i++)
		*(BYTE*)((DWORD)(at)+i) = bytes[i];
}

class CPageProtector
{
public:
	CPageProtector(void*, size_t);
	~CPageProtector();

private:
	void* m_pAt;
	DWORD m_dwOldProtections;
	size_t m_nBytesProtected;
};

// `to` has to be static, otherwise the generated code
// can be weird (an indirection) and end up not working as expected.
void Hook::Install(void* from, void* to, void** original)
{
	// get rights to write to memory
	CPageProtector protector(from, g_nBytesPatchForHook);

	// preserve original
	if (original)
		*original = from;

	// associate the new function pointer with original
	g_mapDetourBack[to] = from;

	// store original bytes
	g_mapOriginalBytes[from] = ReadNBytes<g_nBytesPatchForHook>(from);

	// write payload
	HookBytes_t hookBytes;
	hookBytes[0] = 0xe9;
	*(uintptr_t*)(&hookBytes[1]) = (uintptr_t)(to)-((uintptr_t)(from)+g_nBytesPatchForHook);

	// install payload at prologue
	WriteNBytes(from, hookBytes);
}

void Hook::Remove(void* to)
{
	// get original address
	void* pOriginalAddress = g_mapDetourBack[to];

	// get rights to write to memory
	CPageProtector protector(pOriginalAddress, g_nBytesPatchForHook);

	// get original bytes
	const HookBytes_t& originalBytes = g_mapOriginalBytes[pOriginalAddress];

	// write back original bytes
	WriteNBytes(pOriginalAddress, originalBytes);
}

void Hook::Reinstall(void* to)
{
	Hook::Install(g_mapDetourBack[to], to, nullptr);
}

inline CPageProtector::CPageProtector(void* at, size_t protSize)
{
	m_pAt = at;
	m_nBytesProtected = protSize;
	// no ex even though os rwx protections shouldn't affect samp
	VirtualProtect(at, protSize, PAGE_READWRITE, &m_dwOldProtections);
}

inline CPageProtector::~CPageProtector()
{
	DWORD s;
	VirtualProtect(m_pAt, m_nBytesProtected, m_dwOldProtections, &s);
	(void)s;
}
