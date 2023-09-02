#pragma once

class CPlayer;
class CPlayerList
{
public:
	CPlayerList() = delete;

	DWORD GetPlayerCount() const;
	CPlayer* GetPlayer(DWORD) const;
	const char* GetNameFromID(int) const;
};
