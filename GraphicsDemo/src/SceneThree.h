////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File			:	SceneThree.h/.cpp
//	Description		:	Data for Scene Three
//	Author			:	Hasan 'Qiyn' A. (11/18)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../ext/nclgl/Camera.h"
#include "../../ext/nclgl/HeightMap.h"
#include "../../ext/nclgl/MD5Node.h"

namespace Qiyn
{
	class SceneThree
	{
	public:
		SceneThree(OGLRenderer* r, Camera* c);
		~SceneThree(void);

		void				Update(float msec);
		void				Draw();

	private:
		OGLRenderer*		renderer;
		Camera*				camera;
	};
}
