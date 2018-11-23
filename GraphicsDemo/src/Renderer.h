////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File			:	Renderer.h/.cpp
//	Description		:	Rendering functionality for Scene data
//	Author			:	Hasan 'Qiyn' A. (11/18)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "SceneOne.h"
#include "SceneTwo.h"
#include "SceneThree.h"
#include "SceneFour.h"

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

	void				NextScene();
	void				PreviousScene();
	
	void				ToggleStats() { isStatsActive = !isStatsActive; }
	void				ResetCamera();

protected:
	Camera*				camera;

	//Scene Data Related
	Qiyn::SceneOne*		sceneOne;
	Qiyn::SceneTwo*		sceneTwo;
	Qiyn::SceneThree*	sceneThree;
	//Qiyn::SceneFour*	sceneFour;
	
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

