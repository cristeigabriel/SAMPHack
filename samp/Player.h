#pragma once

struct Vector3D;
class CPlayer
{
public:
	CPlayer() = delete;

	bool IsInCar() const;

	void GetOrigin(Vector3D*) const;
	void GetBonePosition(int, Vector3D*) const;
	long double GetDistanceToLocal() const;
};
