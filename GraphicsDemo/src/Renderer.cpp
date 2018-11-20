#include "Renderer.h"

Renderer::Renderer(Window &parent, Timer* timer) : OGLRenderer(parent), timer(timer)
{
	//InitStats();
	//InitSkybox();
	//InitWater();

	camera = new Camera();
	camera->SetPosition(Vector3(RAW_WIDTH*HEIGHTMAP_X / 2.0f, 500.0f, RAW_WIDTH*HEIGHTMAP_X));
	light = new Light(Vector3(RAW_WIDTH*HEIGHTMAP_X / 2.0f + 400.0f, 1000.0f, RAW_WIDTH*HEIGHTMAP_X + 200.0f),
		Vector4(1, 1, 1, 1), 5500.0f);
	
	InitHellknight();
	InitShadow();
	InitHeightMap();

	init = true;
	
	//glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer(void) 
{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	delete camera;
	//delete basicFont;
	//delete fpsTextMesh;
	delete heightMap;
	//delete quad;
	//delete reflectShader;
	//delete skyboxShader;
	//delete lightShader;
	delete light;
	//delete hellShader;
	delete hellData;
	delete hellNode;
	//delete root;
	delete sceneShader;
	delete shadowShader;

	currentShader = 0;
}

void Renderer::UpdateScene(float msec) 
{
	//fps = roundf(timer->GetFPS() * 10) / 10;
	//fpsText = "FPS: " + (to_string((short)fps));

	//waterRotate += msec / 1000.0f;

	//viewMatrix = camera->BuildViewMatrix();
	//frameFrustum.FromMatrix(projMatrix * viewMatrix);

	camera->UpdateCamera(msec);
	hellNode->Update(msec);

	//root->Update(msec);
}

void Renderer::RenderScene() 
{
	//timer->Update();

	//BuildNodeLists(root);
	//SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	//DrawSkybox();
	//DrawHeightMap();
	
	//glEnable(GL_DEPTH_TEST);
	
	DrawShadowScene();
	DrawCombinedScene();
	//DrawWater();

	//if (isStatsActive)
	//	DrawStats();
	
	//DrawNodes();

	//glUseProgram(0);
	SwapBuffers();
	//ClearNodeLists();
}

#pragma region Stats Display

void Renderer::InitStats()
{
	textTextureShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");

	if (!textTextureShader->LinkProgram())
		return;

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

void Renderer::ToggleStats()
{
	isStatsActive = !isStatsActive;
}

#pragma endregion

#pragma region Scene Node

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		nodeList.push_back(from);
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists()
{
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes()
{
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i)
	{
		DrawNode((*i));
	}
}

void Renderer::DrawNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"),
			1, false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"),
			1, (float*)&n->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"),
			(int)n->GetMesh()->GetTexture());

		n->Draw(*this);
	}
}

void Renderer::ClearNodeLists()
{
	nodeList.clear();
}

#pragma endregion 

#pragma region Scene Contents

void Renderer::InitSkybox()
{
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl",
		SHADERDIR"SkyboxFragment.glsl");

	if (!skyboxShader->LinkProgram())
		return;

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
	);

	if (!cubeMap)
		return;
}

void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	
	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_TRUE);
}

void Renderer::InitHeightMap()
{
	//lightShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"BumpFragment.glsl");
	//if (!lightShader->LinkProgram()) return;

	heightMap = new HeightMap(TEXTUREDIR"/terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!heightMap->GetTexture() ||
		!heightMap->GetBumpMap())
		return;

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	/*light = new Light(Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f)),
		Vector4(0.9f, 0.9f, 1.0f, 1.0f), (RAW_WIDTH*HEIGHTMAP_X) / 2.0f);*/

	glEnable(GL_DEPTH_TEST);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	
}

void Renderer::DrawHeightMap()
{
	glEnable(GL_DEPTH_TEST);

	SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);
	//glDisable(GL_DEPTH_TEST);
}

void Renderer::InitWater()
{
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"ReflectFragment.glsl");

	if (!reflectShader->LinkProgram())
		return;

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!quad->GetTexture())
		return;

	SetTextureRepeating(quad->GetTexture(), true);

	waterRotate = 0.0f;
}

void Renderer::DrawWater()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	SetCurrentShader(reflectShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH*HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 3.0f;
	float heightZ = (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	quad->Draw();

	textureMatrix.ToIdentity();

	glUseProgram(0);

	glDisable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ZERO);
}

void Renderer::InitHellknight()
{
	//hellShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	//if (!hellShader->LinkProgram())	return;

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellData->AddAnim(MESHDIR"idle2.md5anim");

	hellNode = new MD5Node(*hellData);
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");
}

void Renderer::DrawHellknight()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	SetCurrentShader(hellShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	
	modelMatrix = (
		Matrix4::Translation(Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f) + 400.0f, 300.0f, (RAW_WIDTH*HEIGHTMAP_X / 2.0f) - 200.0f)) *
		Matrix4::Rotation(45, Vector3(0, 1, 0)) *
		Matrix4::Scale(Vector3(5.0f, 5.0f, 5.0f))
		);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	UpdateShaderMatrices();

	hellNode->Draw(*this);

	glUseProgram(0);

	glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
}

#pragma endregion

#pragma region Shadow

void Renderer::InitShadow()
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

void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), 
		Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f), 1, (RAW_WIDTH*HEIGHTMAP_X / 2.0f)));
	textureMatrix = biasMatrix * (projMatrix*viewMatrix);

	UpdateShaderMatrices();

	DrawShadowHeightMap();
	DrawShadowHellknight();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene()
{
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	DrawShadowHeightMap();
	DrawShadowHellknight();

	glUseProgram(0);
}

void Renderer::DrawShadowHeightMap()
{
	modelMatrix.ToIdentity();

	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	/*glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
*/
	//UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawShadowHellknight()
{
	modelMatrix = (
		Matrix4::Translation(Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f) + 400.0f, 300.0f, (RAW_WIDTH*HEIGHTMAP_X / 2.0f) - 200.0f)) *
		Matrix4::Rotation(45, Vector3(0, 1, 0)) *
		Matrix4::Scale(Vector3(5.0f, 5.0f, 5.0f))
		);

	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	hellNode->Draw(*this);
}

#pragma endregion