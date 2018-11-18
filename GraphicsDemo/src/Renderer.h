#pragma once

#include "Timer.h"
#include "../../ext/nclgl/OGLRenderer.h"
#include "../../ext/nclgl/Camera.h"
#include "../../ext/nclgl/Frustum.h"
#include "../../ext/nclgl/HeightMap.h"
#include "../../ext/nclgl/MD5Node.h"
#include "../../ext/nclgl/SceneNode.h"
#include "../../ext/nclgl/TextMesh.h"
#include <algorithm>
#include <string>
#include <math.h>

#define SHADOWSIZE 2048

class Renderer : public OGLRenderer
{
public:
						Renderer(Window &parent, Timer* timer);
	virtual				~Renderer(void);

	virtual void		UpdateScene(float msec);
	virtual void		RenderScene();

	void				ToggleStats();
	void				DrawStats(const float size = 16.0f);

protected:
	//Rendering/Culling Related
	Camera*				camera;
	Frustum				frameFrustum;

	//Scene Management Related
	void				BuildNodeLists(SceneNode* from);
	void				SortNodeLists();
	void				DrawNodes();
	void				DrawNode(SceneNode* n);
	void				ClearNodeLists();
	
	SceneNode*			root;
	vector<SceneNode*>	nodeList;

	//Stats Related
	Timer*				timer;
	Font*				basicFont;
	Shader*				textTextureShader;
	bool				isStatsActive;
	TextMesh*			fpsTextMesh;
	float				fps;
	string				fpsText;
	Vector3				fpsTextPosition;

	//Scene Content Related
	void				DrawHeightMap();
	void				DrawWater();
	void				DrawSkybox();

	Shader*				lightShader;
	Shader*				reflectShader;
	Shader*				skyboxShader;
	HeightMap*			heightMap;
	Mesh*				quad;
	Light*				light;
	GLuint				cubeMap;
	float				waterRotate;

	//Hellknight Related
	void				InitHellknight();
	void				DrawShadowScene();
	void				DrawCombinedScene();
	void				DrawTexturedHellknight();
	void				DrawMesh();

	Shader*				shadowSceneShader;
	Shader*				shadowShader;
	Shader*				textureShader;
	GLuint				shadowTex;
	GLuint				shadowFBO;
	MD5FileData*		hellData;
	MD5Node*			hellNode;

};

