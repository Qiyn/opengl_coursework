////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File			:	SceneThree.h/.cpp
//	Description		:	Data for Scene Three
//	Author			:	Hasan 'Qiyn' A. (11/18)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Particle.h"
#include "../../ext/nclgl/Camera.h"
#include "../../ext/nclgl/HeightMap.h"
#include "../../ext/nclgl/MD5Node.h"
#include <vector>
#include <algorithm>

#define MAX_PARTICLES 500

using namespace std;

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

		//Skybox/Cube Map Related
		void				InitSkybox();
		void				DrawSkybox();

		Shader*				skyboxShader;
		GLuint				cubeMap;

		Mesh*				skyQuad;

		//Particle System Related
		void				InitParticles();
		void				UpdateParticles(float msec);
		void				DrawParticles();

		int					FindUnusedParticle();
		void				SortParticles();

		Particle			particles[MAX_PARTICLES];
		int					lastUsedParticle = 0;
		
		GLuint				vertexArray;
		
		Shader*				particleShader;
		GLuint				cameraRightWorldspace;
		GLuint				cameraUpWorldspace;
		GLuint				viewProjMatrix;

		Matrix4				viewProjMatrixValues;

		GLuint				particleTexID;
		GLuint				particleTex;


		Mesh*				particleQuad;

		GLfloat* g_particule_position_size_data = new GLfloat[MAX_PARTICLES * 4];
		GLubyte* g_particule_color_data = new GLubyte[MAX_PARTICLES * 4];

		GLfloat g_vertex_buffer_data[12] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
		};

		GLuint billboard_vertex_buffer;
		GLuint particles_position_buffer;
		GLuint particles_color_buffer;

		int ParticlesCount;
	};
}
