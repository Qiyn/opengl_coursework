#pragma once

#include "../../ext/nclgl/Vector3.h"
#include "../../ext/nclgl/Vector4.h"

struct Particle
{
	Vector3 Position = Vector3();
	Vector3 Velocity = Vector3();
	unsigned char r, g, b, a = 1;
	float Size, Angle, Weight = 1.0f;
	float Life = 1.0f;
	float CameraDistance = 1.0f;

	bool operator<(const Particle& that) const
	{
		return this->CameraDistance > that.CameraDistance;
	}
};