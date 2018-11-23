#pragma once

#include "../../ext/nclgl/Vector3.h"
#include "../../ext/nclgl/Vector4.h"

struct Particle
{
	Vector3 Position;
	Vector3 Velocity;
	Vector4 Colour;
	float Size, Angle, Weight;
	float Life;
	float CameraDistance;

	bool operator<(const Particle& that) const
	{
		return this->CameraDistance > that.CameraDistance;
	}
};