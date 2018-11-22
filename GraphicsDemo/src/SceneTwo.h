////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File			:	SceneTwo.h/.cpp
//	Description		:	Data for Scene Two
//	Author			:	Hasan 'Qiyn' A. (11/18)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../ext/nclgl/Camera.h"
#include "../../ext/nclgl/HeightMap.h"
#include "../../ext/nclgl/MD5Node.h"

namespace Qiyn
{
	class SceneTwo
	{
	public:
							SceneTwo(OGLRenderer* r, Camera* c);
							~SceneTwo(void);

		void				Update(float msec);
		void				Draw();
	
	private:
		OGLRenderer*		renderer;
		Camera*				camera;

		//Skybox/Cube Map Related
		void				InitSkybox();
		void				DrawSkybox();

		Shader*				skyboxShader;
		GLuint				cubeMap;

		Mesh*				quad;
	};
}
