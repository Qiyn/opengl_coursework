#include "Renderer.h"

Renderer::Renderer(Window &parent, Timer* timer) : OGLRenderer(parent), timer(timer)
{
	camera = new Camera();
	camera->SetPosition(Vector3(1910.0f, 500.0f, 3400.0f));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,(float)width / (float)height, 45.0f);
	
	sceneOne = new Qiyn::SceneOne(this, camera);
	sceneTwo = new Qiyn::SceneTwo(this, camera);
	sceneThree = new Qiyn::SceneThree(this, camera);

	InitStats();

	init = true;
}

Renderer::~Renderer(void) 
{
	delete camera;

	delete sceneOne;
	delete sceneTwo;
	delete sceneThree;

	delete basicFont;
	delete fpsTextMesh;
	
	currentShader = 0;
}


void Renderer::UpdateScene(float msec) 
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	switch (activeSceneIndex)
	{
	case 0:	//SCENE #1
		sceneOne->Update(msec);
		break;
	case 1:	//SCENE #2
		sceneTwo->Update(msec);
		break;
	case 2:	//SCENE 3
		sceneThree->Update(msec);
		break;
	}

	fps = roundf(timer->GetFPS() * 10) / 10;
	fpsText = "FPS: " + (to_string((short)fps));
}

void Renderer::RenderScene() 
{
	timer->Update();	//Has to be placed in Render function to measure time between calls

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	switch (activeSceneIndex)
	{
	case 0:	//SCENE #1
		sceneOne->Draw();
		break;
	case 1:	//SCENE #2
		sceneTwo->Draw();
		break;
	case 2:	//SCENE #3
		sceneThree->Draw();
		break;
	}

	if (isStatsActive)
		DrawStats();
	
	glUseProgram(0);
	SwapBuffers();
}


void Renderer::NextScene()
{
	if (activeSceneIndex + 1 > 2)
		activeSceneIndex = 0;
	else
		activeSceneIndex++;
}

void Renderer::PreviousScene()
{
	if (activeSceneIndex - 1 < 0)
		activeSceneIndex = 2;
	else
		activeSceneIndex--;
}


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
