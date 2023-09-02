#pragma once

struct Vector3D
{
	float v[3];

	float operator[](size_t) const;
	float& operator[](size_t);
};
