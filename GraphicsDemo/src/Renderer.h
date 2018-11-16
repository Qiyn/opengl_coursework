#pragma once
#include "../../ext/nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);
	virtual void RenderScene();

protected:
	Mesh* triangle;
};

