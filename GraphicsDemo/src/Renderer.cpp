#include "Renderer.h"

Renderer::Renderer(Window &parent, Timer* timer) : 
	OGLRenderer(parent), 
	timer(timer) 
{
	camera = new Camera();

	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!currentShader->LinkProgram())
	{
		//Report Error in Shader
		return;
	}

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, 
							SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	//TODO: Check font loading 

	init = true;
	isStatsActive = true;

	fpsTextPosition = Vector3(25.0f, 25.0f, 0.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

Renderer::~Renderer(void) 
{
	delete camera;
	delete basicFont;
}

void Renderer::UpdateScene(float msec) 
{
	camera->UpdateCamera(msec);

	fps = roundf(timer->GetFPS() * 10) / 10;
	fpsText = "FPS: " + (to_string((short)fps));
}

void Renderer::RenderScene() 
{
	timer->Update();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	if (isStatsActive)
		DrawStats();

	glUseProgram(0);

	SwapBuffers();
}

#pragma region Stats Display

void Renderer::ToggleStats()
{
	isStatsActive = !isStatsActive;
}

void Renderer::DrawStats(const float size) 
{
	TextMesh* mesh = new TextMesh(fpsText, *basicFont);

	modelMatrix = Matrix4::Translation(Vector3(fpsTextPosition.x, height - fpsTextPosition.y, fpsTextPosition.z)) * Matrix4::Scale(Vector3(size, size, 1));
	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);

	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh;
}

#pragma endregion
