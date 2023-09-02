#include "pch.h"
#include "Math.h"

float Vector3D::operator[](size_t idx) const
{
	return v[idx];
}

float& Vector3D::operator[](size_t idx)
{
	return v[idx];
}
