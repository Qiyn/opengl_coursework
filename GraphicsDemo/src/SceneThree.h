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

#define SHADOWSIZE 2048

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

		Light*				light;

		//Skybox/Cube Map Related
		void				InitSkybox();
		void				DrawSkybox();
		void				InitWater();
		void				DrawWater();

		Shader*				reflectShader;
		Shader*				skyboxShader;
		Mesh*				quad;
		GLuint				cubeMap;
		float				waterRotate;

		//Height Map Related
		void				InitHeightMap();

		HeightMap*			heightMap;

		//Hellknight Related
		void				InitHellknight();

		MD5FileData*		hellData;
		MD5Node*			hellNode;

		//Shadow Related
		void				InitShadow();
		void				DrawShadowScene();
		void				DrawCombinedScene();

		void				DrawShadowHeightMap();
		void				DrawShadowHellknight();

		Shader*				sceneShader;
		Shader*				shadowShader;
		GLuint				shadowTex;
		GLuint				shadowFBO;
	};
}
