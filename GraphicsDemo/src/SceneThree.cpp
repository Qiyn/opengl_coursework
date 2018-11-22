#include "SceneThree.h"

using namespace Qiyn;

SceneThree::SceneThree(OGLRenderer* r, Camera* c) : renderer(r), camera(c)
{
	InitSkybox();
}

SceneThree::~SceneThree(void)
{
	delete skyboxShader;
	delete skyQuad;
}


void SceneThree::Update(float msec)
{

}

void SceneThree::Draw()
{
	DrawSkybox();
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