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
		TEXTUREDIR"lightblue/right-s.png", TEXTUREDIR"lightblue/left-s.png", TEXTUREDIR"lightblue/top-s.png",
		TEXTUREDIR"lightblue/bot-s.png", TEXTUREDIR"lightblue/front-s.png", TEXTUREDIR"lightblue/back-s.png",
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

	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"cubeTex"), 0);

	quad->Draw();

	glUseProgram(0);

	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_TRUE);
}