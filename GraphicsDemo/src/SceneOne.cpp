#include "SceneOne.h"

using namespace Qiyn;

SceneOne::SceneOne(OGLRenderer* r, Camera* c) : renderer(r), camera(c)
{
	light = new Light(
		Vector3(900.0f, 2000.0f, 3500.0f),
		Vector4(1, 1, 1, 1),
		4000.0f
	);

	InitSkybox();
	InitHeightMap();
	InitWater();
	InitHellknight();
	InitShadow();
}

SceneOne::~SceneOne(void)
{
	delete light;

	delete heightMap;
	delete quad;
	delete hellData;
	delete hellNode;

	delete skyboxShader;
	delete reflectShader;
	delete sceneShader;
	delete shadowShader;

	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
}


void SceneOne::Update(float msec)
{
	waterRotate += msec / 1000.0f;

	hellNode->Update(msec);
}

void SceneOne::Draw()
{
	DrawSkybox();

	DrawShadowScene();
	DrawCombinedScene();

	DrawWater();
}


void SceneOne::InitHeightMap()
{
	heightMap = new HeightMap(TEXTUREDIR"/terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!heightMap->GetTexture() || !heightMap->GetBumpMap()) return;

	renderer->SetTextureRepeating(heightMap->GetTexture(), true);
	renderer->SetTextureRepeating(heightMap->GetBumpMap(), true);
}

void SceneOne::InitSkybox()
{
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	if (!skyboxShader->LinkProgram()) return;

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg",
		TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
	);
	if (!cubeMap) return;
}

void SceneOne::InitWater()
{
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"ReflectFragment.glsl");

	if (!reflectShader->LinkProgram())
		return;

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!quad->GetTexture()) return;

	renderer->SetTextureRepeating(quad->GetTexture(), true);

	waterRotate = 0.0f;
}

void SceneOne::InitHellknight()
{
	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellData->AddAnim(MESHDIR"idle2.md5anim");

	hellNode = new MD5Node(*hellData);
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");
}

void SceneOne::InitShadow()
{
	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");
	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram()) return;

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SceneOne::DrawSkybox()
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

void SceneOne::DrawWater()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	renderer->SetCurrentShader(reflectShader);
	renderer->SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH*HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 3.0f;
	float heightZ = (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f);

	renderer->modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	renderer->textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	renderer->UpdateShaderMatrices();

	quad->Draw();

	renderer->textureMatrix.ToIdentity();

	glUseProgram(0);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ZERO);
}

void SceneOne::DrawShadowScene()
{
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	renderer->SetCurrentShader(shadowShader);

	renderer->viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f), 1, (RAW_WIDTH*HEIGHTMAP_X / 2.0f)));
	renderer->textureMatrix = biasMatrix * (renderer->projMatrix*renderer->viewMatrix);

	renderer->UpdateShaderMatrices();

	DrawShadowHeightMap();
	DrawShadowHellknight();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, renderer->width, renderer->height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneOne::DrawCombinedScene()
{
	renderer->SetCurrentShader(sceneShader);

	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	renderer->SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->UpdateShaderMatrices();

	DrawShadowHeightMap();
	DrawShadowHellknight();

	renderer->textureMatrix.ToIdentity();	//Restores stats display

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
}

void SceneOne::DrawShadowHeightMap()
{
	renderer->modelMatrix.ToIdentity();

	Matrix4 tempMatrix = renderer->textureMatrix * renderer->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "modelMatrix"), 1, false, *&renderer->modelMatrix.values);

	heightMap->Draw();
}

void SceneOne::DrawShadowHellknight()
{
	renderer->modelMatrix = (
		Matrix4::Translation(Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f) + 400.0f, 300.0f, (RAW_WIDTH*HEIGHTMAP_X / 2.0f) - 200.0f)) *
		Matrix4::Rotation(45, Vector3(0, 1, 0)) *
		Matrix4::Scale(Vector3(5.0f, 5.0f, 5.0f))
		);

	Matrix4 tempMatrix = renderer->textureMatrix * renderer->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(), "modelMatrix"), 1, false, *&renderer->modelMatrix.values);

	hellNode->Draw(*renderer);
}
