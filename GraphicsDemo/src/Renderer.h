#pragma once

#include "Timer.h"
#include "../../ext/nclgl/OGLRenderer.h"
#include "../../ext/nclgl/Camera.h"
#include "../../ext/nclgl/Frustum.h"
#include "../../ext/nclgl/HeightMap.h"
#include "../../ext/nclgl/MD5Mesh.h"
#include "../../ext/nclgl/MD5Node.h"
#include "../../ext/nclgl/SceneNode.h"
#include "../../ext/nclgl/TextMesh.h"
#include <algorithm>
#include <string>
#include <math.h>

#define SHADOWSIZE 2048
#define SCENE_COUNT 3

class Renderer : public OGLRenderer
{
public:
						Renderer(Window &parent, Timer* timer);
	virtual				~Renderer(void);

	virtual void		UpdateScene(float msec);
	virtual void		RenderScene();

	void				ToggleStats() { isStatsActive = !isStatsActive; }
	void				NextScene() { activeSceneIndex++; activeSceneIndex %= SCENE_COUNT; }
	void				PreviousScene() { activeSceneIndex--; activeSceneIndex %= SCENE_COUNT; }

protected:

	signed char		activeSceneIndex = 0;

	//Rendering/Culling Related
	Camera*				camera;
	Frustum				frameFrustum;

	////Scene Management Related
	void				BuildNodeLists(SceneNode* from);
	void				SortNodeLists();
	void				DrawNodes();
	void				DrawNode(SceneNode* n);
	void				ClearNodeLists();
	
	SceneNode*			root;
	vector<SceneNode*>	nodeList;

	//Stats Related
	void				InitStats();
	void				DrawStats(const float size = 16.0f);

	Timer*				timer;
	Font*				basicFont;
	Shader*				textTextureShader;
	bool				isStatsActive;
	TextMesh*			fpsTextMesh;
	float				fps;
	string				fpsText;
	Vector3				fpsTextPosition;

	//Scene Content Related
	void				InitSkybox();
	void				DrawSkybox();
	void				InitHeightMap();
	void				DrawHeightMap();
	void				InitWater();
	void				DrawWater();

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
	void				DrawHellknight();

	Shader*				hellShader;
	MD5FileData*		hellData;
	MD5Node*			hellNode;

	//Shadow Related
	void				InitShadow();
	void				DrawShadowScene();
	void				DrawCombinedScene();

	void				DrawShadowHeightMap();
	void				DrawShadowHellknight();

	Shader*				sceneShader;
	Shader*				shadowShader;
	GLuint				shadowTex;
	GLuint				shadowFBO;
};

