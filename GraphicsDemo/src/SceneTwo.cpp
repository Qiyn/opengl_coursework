#include "SceneTwo.h"

using namespace Qiyn;

SceneTwo::SceneTwo(OGLRenderer* r, Camera* c) : renderer(r), camera(c)
{
	InitSkybox();

	screenQuad = Mesh::GenerateQuad();
	// Need to make an empty constructor for the Light class ...
	pointLights = new Light[LIGHTNUM * LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light & l = pointLights[(x * LIGHTNUM) + z];

			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));

			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (RAW_WIDTH * HEIGHTMAP_X / LIGHTNUM);
			l.SetRadius(radius);
		}
	}

	rotation = 0.0f;

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	renderer->SetTextureRepeating(heightMap->GetTexture(), true);
	renderer->SetTextureRepeating(heightMap->GetBumpMap(), true);

	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"ico.obj")) {
		return;
	}

	sceneShader = new Shader(SHADERDIR"BumpVertex.glsl",
		SHADERDIR"bufferFragment.glsl");
	if (!sceneShader->LinkProgram()) {
		return;
	}

	combineShader = new Shader(SHADERDIR"combinevertex.glsl",
		SHADERDIR "combinefragment.glsl");
	if (!combineShader->LinkProgram()) {
		return;
	}

	pointlightShader = new Shader(SHADERDIR"pointlightvertex.glsl",
		SHADERDIR"pointlightfragment.glsl");
	if (!pointlightShader->LinkProgram()) {
		return;
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	// Generate our scene depth texture ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);*/
}

SceneTwo::~SceneTwo(void)
{
	delete skyboxShader;
	delete skyQuad;

	delete sceneShader;
	delete combineShader;
	delete pointlightShader;

	delete heightMap;
	delete sphere;
	delete screenQuad;
	delete[] pointLights;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
}


void SceneTwo::Update(float msec)
{
	rotation = msec * 0.01f;
}

void SceneTwo::Draw()
{
	DrawSkybox();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	FillBuffers();
	DrawPointLights();
	CombineBuffers();
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

	skyQuad = Mesh::GenerateQuad();
}

void SceneTwo::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	renderer->SetCurrentShader(skyboxShader);
	renderer->UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"cubeTex"), 0);

	skyQuad->Draw();

	glUseProgram(0);

	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_TRUE);
}


void SceneTwo::GenerateScreenTexture(GLuint & into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0,
		depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,
		renderer->width, renderer->height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneTwo::FillBuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	renderer->SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"bumpTex"), 1);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)renderer->width / (float)renderer->height, 45.0f);
	renderer->modelMatrix.ToIdentity();
	renderer->UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneTwo::DrawPointLights()
{
	renderer->SetCurrentShader(pointlightShader);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"depthTex"), 3);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"pixelSize"), 1.0f / renderer->width, 1.0f / renderer->height);

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light &l = pointLights[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();

			renderer->modelMatrix =
				pushMatrix *
				Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
				popMatrix *
				Matrix4::Translation(l.GetPosition()) *
				Matrix4::Scale(Vector3(radius, radius, radius));

			l.SetPosition(renderer->modelMatrix.GetPositionVector());

			renderer->SetShaderLight(l);

			renderer->UpdateShaderMatrices();

			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			if (dist < radius) {// camera is inside the light volume !
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}

			sphere->Draw();
		}
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void SceneTwo::CombineBuffers()
{
	renderer->SetCurrentShader(combineShader);

	renderer->projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	renderer->UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"emissiveTex"), 3);
	glUniform1i(glGetUniformLocation(renderer->GetCurrentShader()->GetProgram(),
		"specularTex"), 4);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	screenQuad->Draw();

	glUseProgram(0);
}
