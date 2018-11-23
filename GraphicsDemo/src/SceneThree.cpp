#include "SceneThree.h"

using namespace Qiyn;

SceneThree::SceneThree(OGLRenderer* r, Camera* c) : renderer(r), camera(c)
{
	InitSkybox();

	InitParticles();

	renderer->projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)renderer->width / (float)renderer->height, 45.0f);
}

SceneThree::~SceneThree(void)
{
	delete skyboxShader;
	delete skyQuad;

	delete particleShader;
	delete particleQuad;

	delete[] g_particule_position_size_data;

	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteTextures(1, &particleTex);
	glDeleteVertexArrays(1, &vertexArray);
}


void SceneThree::Update(float msec)
{
	UpdateParticles(msec);
}

void SceneThree::Draw()
{
	DrawSkybox();

	DrawParticles();
}


void SceneThree::InitSkybox()
{
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	if (!skyboxShader->LinkProgram()) return;

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"red/bkg1_right1.png", TEXTUREDIR"red/bkg1_left2.png", TEXTUREDIR"red/bkg1_top3.png",
		TEXTUREDIR"red/bkg1_bottom4.png", TEXTUREDIR"red/bkg1_front5.png", TEXTUREDIR"red/bkg1_back6.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
	);
	if (!cubeMap) return;

	skyQuad = Mesh::GenerateQuad();
}

void SceneThree::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	renderer->SetCurrentShader(skyboxShader);
	renderer->UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"cubeTex"), 2);

	skyQuad->Draw();

	glUseProgram(0);

	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_TRUE);
}


void SceneThree::InitParticles()
{
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	particleShader = new Shader(SHADERDIR"ParticleVertex.glsl", SHADERDIR"ParticleFragment.glsl");
	if (!particleShader->LinkProgram()) return;

	cameraRightWorldspace = glGetUniformLocation(particleShader->GetProgram(), "CameraRight_worldspace");
	cameraUpWorldspace = glGetUniformLocation(particleShader->GetProgram(), "CameraUp_worldspace");
	viewProjMatrix = glGetUniformLocation(particleShader->GetProgram(), "VP");

	particleTexID = glGetUniformLocation(particleShader->GetProgram(), "myTextureSampler");

	for (int i = 0; i < MAX_PARTICLES; i++) 
	{
		particles[i].Life = -1.0f;
		particles[i].CameraDistance = -1.0f;
	}

	particleQuad = Mesh::GenerateQuad();
	particleQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"star1.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	particleTex = particleQuad->GetTexture();

	
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

void SceneThree::UpdateParticles(float dt)
{
	dt = dt / 100;

	viewProjMatrixValues = camera->BuildViewMatrix() * renderer->projMatrix;

	// Generate 10 new particule each millisecond,
		// but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
		// newparticles will be huge and the next frame even longer.
	int newparticles = (int)(dt*10000.0);
	if (newparticles > (int)(0.016f*10000.0))
		newparticles = (int)(0.016f*10000.0);

	for (int i = 0; i < newparticles; i++) {
		int particleIndex = FindUnusedParticle();
		particles[particleIndex].Life = 5.0f; // This particle will live 5 seconds.
		particles[particleIndex].Position = Vector3(0, 0, -20.0f);

		float spread = 1.5f;
		Vector3 maindir = Vector3(0.0f, 10.0f, 0.0f);
		// Very bad way to generate a random direction; 
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc)
		Vector3 randomdir = Vector3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);

		particles[particleIndex].Velocity = maindir + randomdir * spread;


		// Very bad way to generate a random color
		particles[particleIndex].Colour.x = rand() % 256;
		particles[particleIndex].Colour.y = rand() % 256;
		particles[particleIndex].Colour.z = rand() % 256;
		particles[particleIndex].Colour.w = (rand() % 256) / 3;

		particles[particleIndex].Size = (rand() % 1000) / 2000.0f + 0.1f;
	}


	// Simulate all particles
	ParticlesCount = 0;
	for (int i = 0; i < MAX_PARTICLES; i++) {

		Particle& p = particles[i]; // shortcut

		if (p.Life > 0.0f) {

			// Decrease life
			p.Life -= dt;
			if (p.Life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				p.Velocity += Vector3(0.0f, -9.81f, 0.0f) * (float)dt * 0.5f;
				p.Position += p.Velocity * (float)dt;
				p.CameraDistance = (p.Position - camera->GetPosition()).Length();
				//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

				// Fill the GPU buffer
				g_particule_position_size_data[4 * ParticlesCount + 0] = p.Position.x;
				g_particule_position_size_data[4 * ParticlesCount + 1] = p.Position.y;
				g_particule_position_size_data[4 * ParticlesCount + 2] = p.Position.z;

				g_particule_position_size_data[4 * ParticlesCount + 3] = p.Size;

				g_particule_color_data[4 * ParticlesCount + 0] = p.Colour.x;
				g_particule_color_data[4 * ParticlesCount + 1] = p.Colour.y;
				g_particule_color_data[4 * ParticlesCount + 2] = p.Colour.z;
				g_particule_color_data[4 * ParticlesCount + 3] = p.Colour.w;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.CameraDistance = -1.0f;
			}

			ParticlesCount++;

		}
	}

	SortParticles();
}

void SceneThree::DrawParticles()
{
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	glUseProgram(particleShader->GetProgram());

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particleTex);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(particleTexID, 0);

	// Same as the billboards tutorial
	glUniform3f(cameraRightWorldspace, camera->BuildViewMatrix().values[0], camera->BuildViewMatrix().values[1], camera->BuildViewMatrix().values[2]);
	glUniform3f(cameraUpWorldspace, camera->BuildViewMatrix().values[4], camera->BuildViewMatrix().values[5], camera->BuildViewMatrix().values[6]);

	glUniformMatrix4fv(viewProjMatrix, 1, GL_FALSE, &viewProjMatrixValues.values[0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size : x + y + z + size => 4
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size : r + g + b + a => 4
		GL_UNSIGNED_BYTE,                 // type
		GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// These functions are specific to glDrawArrays*Instanced*.
	// The first parameter is the attribute buffer we're talking about.
	// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
	// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
	// but faster.
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

int SceneThree::FindUnusedParticle()
{
	for (int i = lastUsedParticle; i < MAX_PARTICLES; i++) {
		if (particles[i].Life < 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++) {
		if (particles[i].Life < 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void SceneThree::SortParticles()
{
	std::sort(&particles[0], &particles[MAX_PARTICLES]);
}