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

#define LIGHTNUM 8
#define POST_PASSES 10

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

		Mesh*				skyQuad;
		
		//Deferred Rendering Related
		void				InitElse();		//TO REMOVE
		void				FillBuffers();
		void				DrawPointLights();
		void				CombineBuffers();
		void				GenerateScreenTexture(GLuint &into, bool depth = false);

		Shader*				sceneShader; // Shader to fill our GBuffers
		Shader*				pointlightShader; // Shader to calculate lighting
		Shader*				combineShader; // shader to stick it all together

		Light*				pointLights; // Array of lighting data
		Mesh*				heightMap; // Terrain !
		OBJMesh*			sphere; // Light volume
		Mesh*				screenQuad; // To draw a full - screen quad

		float				rotation; // How much to increase rotation by

		GLuint				bufferFBO; // FBO for our G- Buffer pass
		GLuint				bufferColourTex; // Albedo goes here
		GLuint				bufferNormalTex; // Normals go here
		GLuint				bufferDepthTex; // Depth goes here

		GLuint				pointLightFBO; // FBO for our lighting pass
		GLuint				lightEmissiveTex; // Store emissive lighting
		GLuint				lightSpecularTex; // Store specular lighting

		//Post-Processing Related
		void				InitPostProcessing();
		void				DrawPostProcessing();

		Shader* processShader;
		Mesh* processQuad;

		GLuint processFBO;
	};
}
