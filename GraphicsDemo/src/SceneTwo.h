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
#include "../../ext/nclgl/OBJMesh.h"

#define LIGHTNUM 5

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

		
		//Deferred Rendering Related
		void				InitDeferred();
		void				FillBuffers();
		void				DrawPointLights();
		void				CombineBuffers();
		void				GenerateScreenTexture(GLuint &into, bool depth = false);

		Shader*				sceneShader;
		Shader*				pointlightShader;
		Shader*				combineShader;

		Light*				pointLights;
		Mesh*				heightMap;
		OBJMesh*			sphere;
		Mesh*				screenQuad;

		float				rotation;
		float				scaling;
		float				scalingModifier;

		GLuint				bufferFBO;
		GLuint				bufferColourTex;
		GLuint				bufferNormalTex;
		GLuint				bufferDepthTex;

		GLuint				pointLightFBO;
		GLuint				lightEmissiveTex;
		GLuint				lightSpecularTex;
	};
}
