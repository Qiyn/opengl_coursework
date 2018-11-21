#include "Renderer.h"

Renderer::Renderer(Window &parent, Timer* timer) : OGLRenderer(parent), timer(timer)
{
	InitStats();
	
	sceneOne = new Qiyn::SceneOne();

	camera = new Camera();
	camera->SetPosition(Vector3(1910.0f, 500.0f, 3400.0f));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,(float)width / (float)height, 45.0f);

	init = true;
}

Renderer::~Renderer(void) 
{
	delete camera;

	delete sceneOne;
	
	delete basicFont;
	delete fpsTextMesh;
	
	currentShader = 0;
}

void Renderer::UpdateScene(float msec) 
{
	fps = roundf(timer->GetFPS() * 10) / 10;
	fpsText = "FPS: " + (to_string((short)fps));
	
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	switch (activeSceneIndex)
	{
	case 0:	//SCENE #1
		sceneOne->Update(msec);
		break;
	case 1:	//SCENE #2

		break;
	case 2:	//SCENE 3

		break;
	}
}

void Renderer::RenderScene() 
{
	timer->Update();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	switch (activeSceneIndex)
	{
	case 0:
		sceneOne->Draw(*this, *camera);
		break;
	case 1:

		break;
	case 2:

		break;
	}

	if (isStatsActive)
		DrawStats();
	
	glUseProgram(0);
	SwapBuffers();
}

#pragma region Stats Display

void Renderer::InitStats()
{
	textTextureShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!textTextureShader->LinkProgram()) return;

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	fpsTextMesh = new TextMesh(fpsText, *basicFont);
	fpsTextPosition = Vector3(25.0f, 25.0f, 0.0f);

	isStatsActive = true;
}

void Renderer::DrawStats(const float size) 
{
	TextMesh* mesh = new TextMesh(fpsText, *basicFont);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	
	SetCurrentShader(textTextureShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	modelMatrix = Matrix4::Translation(Vector3(fpsTextPosition.x, height - fpsTextPosition.y, fpsTextPosition.z)) * Matrix4::Scale(Vector3(size, size, 1));
	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);

	UpdateShaderMatrices();
	mesh->Draw();

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	UpdateShaderMatrices();
	
	glUseProgram(0);

	glDepthMask(GL_TRUE);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	delete mesh;
}

#pragma endregion