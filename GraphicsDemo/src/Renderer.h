#pragma once

#include "Timer.h"
#include "../../ext/nclgl/OGLRenderer.h"
#include "../../ext/nclgl/Camera.h"
#include "../../ext/nclgl/TextMesh.h"
#include <string>
#include <math.h>

class Renderer : public OGLRenderer
{
public:
					Renderer(Window &parent, Timer* timer);
	virtual			~Renderer(void);

	virtual void	UpdateScene(float msec);
	virtual void	RenderScene();

	void			ToggleStats();

	void			DrawStats(const float size = 16.0f);

protected:
	Timer*			timer;
	Camera*			camera;

	//Stats display variables
	Font*			basicFont;
	bool			isStatsActive;
	float			fps;
	string			fpsText;
	Vector3			fpsTextPosition;

};

