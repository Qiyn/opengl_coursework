#include "SceneTwo.h"

using namespace Qiyn;

SceneTwo::SceneTwo(OGLRenderer* r, Camera* c) : renderer(r), camera(c)
{
	InitSkybox();
}

SceneTwo::~SceneTwo(void)
{
	delete skyboxShader;
	delete quad;
}


void SceneTwo::Update(float msec)
{

}

void SceneTwo::Draw()
{
	DrawSkybox();
}

void SceneTwo::InitSkybox()
{
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	if (!skyboxShader->LinkProgram()) return;

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg",
		TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
	);
	if (!cubeMap) return;

	quad = Mesh::GenerateQuad();
}

void SceneTwo::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	renderer->SetCurrentShader(skyboxShader);
	renderer->UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);

	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_TRUE);
}