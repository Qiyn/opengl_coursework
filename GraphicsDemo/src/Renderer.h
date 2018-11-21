#pragma once

#include "SceneOne.h"
#include "Timer.h"
#include "../../ext/nclgl/OGLRenderer.h"
#include "../../ext/nclgl/TextMesh.h"
#include <algorithm>
#include <string>
#include <math.h>

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
	Camera*				camera;

	//Scene Data Related
	Qiyn::SceneOne*		sceneOne;
	//Qiyn::SceneTwo*	sceneTwo;
	//Qiyn::SceneThree*	sceneThree;
	
	signed char			activeSceneIndex = 0;

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

};

