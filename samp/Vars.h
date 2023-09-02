#pragma once

class CPlayerList;
class CVars
{
public:
	CVars() = delete;

	void SetNameTagStatus(BYTE);
	const BYTE& GetNameTagStatusEnabled() const;
	BYTE& GetNameTagStatusEnabled();

	CPlayerList* GetPlayerList() const;
};
